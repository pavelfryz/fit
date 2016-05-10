/*  
 *  pdsfw.c - PDS firewall kernel module.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>	

#define PROCF_NAME "pdsfw"
#define PROCF_MAX_SIZE 256

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PDS firewall");
MODULE_AUTHOR("Pavel Fryz/xfryzp00");

//the structure used for procfs
static struct proc_dir_entry *proc_file;
static char *procf_buffer;

//the structure used to register the function
static struct nf_hook_ops nfho;

static LIST_HEAD(rules_desc_list);
static LIST_HEAD(rules_impl_list);

struct fw_rule {
    unsigned int id;
    unsigned int src_ip;
    unsigned int dst_ip;
    unsigned short src_port;
    unsigned short dst_port;
    unsigned char proto;
    unsigned char action;
    struct list_head list;
};

struct fw_rule_char {
    unsigned char value;
    struct list_head sublist;
    struct list_head list;
};

struct fw_rule_short {
    unsigned short value;
    struct list_head sublist;
    struct list_head list;
};

struct fw_rule_int {
    unsigned int value;
    struct list_head sublist;
    struct list_head list;
};

struct fw_action {
    unsigned int id;
    unsigned char action;
    struct list_head list;
};

static unsigned int filter_in(const struct nf_hook_ops *ops, struct sk_buff *skb, 
		const struct net_device *in, const struct net_device *out,
		int (*okfn)(struct sk_buff *)) {
    struct iphdr *ip_header = (struct iphdr *)skb_network_header(skb);
    struct udphdr *udp_header;
    struct tcphdr *tcp_header;
    
    struct list_head *p;
    struct fw_rule_char *proto_rule;
    struct fw_rule_int *src_ip_rule;
    struct fw_rule_int *dst_ip_rule;
    struct fw_rule_short *src_port_rule;
    struct fw_rule_short *dst_port_rule;
    struct fw_action *action;
    unsigned char implicit; 
    
    /**get src and dest ip addresses**/
    unsigned int src_ip = ntohl(ip_header->saddr);
    unsigned int dst_ip = ntohl(ip_header->daddr);
    unsigned short src_port = 0;
    unsigned short dst_port = 0;
    /***get src and dest port number***/
    if (ip_header->protocol==IPPROTO_UDP) {
        udp_header = (struct udphdr *)(skb_transport_header(skb)+20);
        src_port = ntohs(udp_header->source);
        dst_port = ntohs(udp_header->dest);
    } else if (ip_header->protocol == IPPROTO_TCP) {
        tcp_header = (struct tcphdr *)(skb_transport_header(skb)+20);
        src_port = ntohs(tcp_header->source);
        dst_port = ntohs(tcp_header->dest);
    }
    
    /**check protocol**/
    implicit = 1;
    list_for_each(p, &rules_impl_list) {
        proto_rule = list_entry(p, struct fw_rule_char, list);
        if(proto_rule->value == 0 || proto_rule->value == ip_header->protocol){
            implicit = 0;
            break;
        }
    }
    if (implicit) return NF_ACCEPT;
    
    /**check src_ip**/
    implicit = 1;
    list_for_each(p, &proto_rule->sublist) {
        src_ip_rule = list_entry(p, struct fw_rule_int, list);
        if(src_ip_rule->value == 0 || src_ip_rule->value == src_ip){
            implicit = 0;
            break;
        }
    }
    if (implicit) return NF_ACCEPT;
    
    /**check dst_ip**/
    implicit = 1;
    list_for_each(p, &src_ip_rule->sublist) {
        dst_ip_rule = list_entry(p, struct fw_rule_int, list);
        if(dst_ip_rule->value == 0 || dst_ip_rule->value == dst_ip){
            implicit = 0;
            break;
        }
    }
    if (implicit) return NF_ACCEPT;
    
    /**check src_port**/
    implicit = 1;
    list_for_each(p, &dst_ip_rule->sublist) {
        src_port_rule = list_entry(p, struct fw_rule_short, list);
        if(src_port_rule->value == 0 || src_port_rule->value == src_port){
            implicit = 0;
            break;
        }
    }
    if (implicit) return NF_ACCEPT;
    
    /**check dst_port**/
    implicit = 1;
    list_for_each(p, &src_port_rule->sublist) {
        dst_port_rule = list_entry(p, struct fw_rule_short, list);
        if(dst_port_rule->value == 0 || dst_port_rule->value == dst_port){
            implicit = 0;
            break;
        }
    }
    if (implicit) return NF_ACCEPT;
    
    action = list_entry(dst_port_rule->sublist.next, struct fw_action, list);
    return action->action;
}

static int del_action(struct list_head *action, struct fw_rule *rule) {
    struct list_head *pos, *n;
    struct fw_action *act;
    
    list_for_each_safe(pos, n, action) {
        act = list_entry(pos, struct fw_action, list);
        if(rule->id == act->id){
            list_del(pos);
            kfree(act);
            break;
        }
    }
    
    return list_empty(action);
}

static void del_action_list(struct list_head *head) {
    struct list_head *pos, *n;
    struct fw_action *action;
    list_for_each_safe(pos, n, head){
        action = list_entry(pos, struct fw_action, list);
        kfree(action);
    }
}

static void del_dst_port_list(struct list_head *head) {
    struct list_head *pos, *n;
    struct fw_rule_short *port;
    list_for_each_safe(pos, n, head){
        port = list_entry(pos, struct fw_rule_short, list);
        del_action_list(&port->sublist);
        kfree(port);
    }
}

static void del_src_port_list(struct list_head *head) {
    struct list_head *pos, *n;
    struct fw_rule_short *port;
    list_for_each_safe(pos, n, head){
        port = list_entry(pos, struct fw_rule_short, list);
        del_dst_port_list(&port->sublist);
        kfree(port);
    }
}

static void del_dst_ip_list(struct list_head *head) {
    struct list_head *pos, *n;
    struct fw_rule_int *ip;
    list_for_each_safe(pos, n, head){
        ip = list_entry(pos, struct fw_rule_int, list);
        del_src_port_list(&ip->sublist);
        kfree(ip);
    }
}

static void del_src_ip_list(struct list_head *head) {
    struct list_head *pos, *n;
    struct fw_rule_int *ip;
    list_for_each_safe(pos, n, head){
        ip = list_entry(pos, struct fw_rule_int, list);
        del_dst_ip_list(&ip->sublist);
        kfree(ip);
    }
}

static void del_proto_list(struct list_head *head) {
    struct list_head *pos, *n;
    struct fw_rule_char *proto;
    list_for_each_safe(pos, n, head){
        proto = list_entry(pos, struct fw_rule_char, list);
        del_src_ip_list(&proto->sublist);
        kfree(proto);
    }
}

static int del_dst_port(struct list_head *port, struct fw_rule *rule) {
    struct list_head *pos, *n;
    struct fw_rule_short *dst_port;
    struct list_head *ptr;
    struct fw_rule *rule2; 
    int exist_with_prefix = 0;
    
    list_for_each_safe(pos, n, port) {
        dst_port = list_entry(pos, struct fw_rule_short, list);
        if(rule->dst_port == dst_port->value) {
            list_for_each(ptr, &rules_desc_list){
                rule2 = list_entry(ptr, struct fw_rule, list);
                if(rule2->proto == rule->proto && 
                        rule2->src_ip == rule->src_ip &&
                        rule2->dst_ip == rule->dst_ip &&
                        rule2->src_port == rule->src_port &&
                        rule2->dst_port == rule->dst_port){ 
                    exist_with_prefix = 1;
                    break;
                }
            }
            if(!exist_with_prefix){ 
                del_action_list(&dst_port->sublist);
                list_del(pos);
                kfree(dst_port);
            } else if(del_action(&dst_port->sublist, rule)){
                list_del(pos);
                kfree(dst_port);
            }
            break;
        }else if(rule->dst_port == 0) {
            if(del_action(&dst_port->sublist, rule)){
                list_del(pos);
                kfree(dst_port);
            }
        }
    }
    
    return list_empty(port);
}

static int del_src_port(struct list_head *port, struct fw_rule *rule) {
    struct list_head *pos, *n;
    struct fw_rule_short *src_port;
    struct list_head *ptr;
    struct fw_rule *rule2; 
    int exist_with_prefix = 0;
    
    list_for_each_safe(pos, n, port) {
        src_port = list_entry(pos, struct fw_rule_short, list);
        if(rule->src_port == src_port->value) {
            list_for_each(ptr, &rules_desc_list){
                rule2 = list_entry(ptr, struct fw_rule, list);
                if(rule2->proto == rule->proto && 
                        rule2->src_ip == rule->src_ip &&
                        rule2->dst_ip == rule->dst_ip &&
                        rule2->src_port == rule->src_port){ 
                    exist_with_prefix = 1;
                    break;
                }
            }
            if(!exist_with_prefix) { 
                del_dst_port_list(&src_port->sublist);
                list_del(pos);
                kfree(src_port);
            } else if(del_dst_port(&src_port->sublist, rule)){
                list_del(pos);
                kfree(src_port);
            }
            break;
        }else if(rule->src_port == 0) {
            rule->src_port = src_port->value;
            if(del_dst_port(&src_port->sublist, rule)){
                list_del(pos);
                kfree(src_port);
            }
            rule->src_port = 0;
        }
    }
    
    return list_empty(port);
} 

static int del_dst_ip(struct list_head *ip, struct fw_rule *rule) {
    struct list_head *pos, *n;
    struct fw_rule_int *dst_ip;
    struct list_head *ptr;
    struct fw_rule *rule2; 
    int exist_with_prefix = 0;
    
    list_for_each_safe(pos, n, ip) {
        dst_ip = list_entry(pos, struct fw_rule_int, list);
        if(rule->dst_ip == dst_ip->value) {
            list_for_each(ptr, &rules_desc_list){
                rule2 = list_entry(ptr, struct fw_rule, list);
                if(rule2->proto == rule->proto && 
                        rule2->src_ip == rule->src_ip &&
                        rule2->dst_ip == rule->dst_ip){ 
                    exist_with_prefix = 1;
                    break;
                }
            }
            if(!exist_with_prefix) { 
                del_src_port_list(&dst_ip->sublist);
                list_del(pos);
                kfree(dst_ip);
            } else if(del_src_port(&dst_ip->sublist, rule)){
                list_del(pos);
                kfree(dst_ip);
            }
            break;
        }else if(rule->dst_ip == 0) {
            rule->dst_ip = dst_ip->value;
            if(del_src_port(&dst_ip->sublist, rule)){
                list_del(pos);
                kfree(dst_ip);
            }
            rule->dst_ip = 0;
        }
    }
    
    return list_empty(ip);
} 
 
static int del_src_ip(struct list_head *ip, struct fw_rule *rule) {
    struct list_head *pos, *n;
    struct fw_rule_int *src_ip;
    struct list_head *ptr;
    struct fw_rule *rule2; 
    int exist_with_prefix = 0;
    
    list_for_each_safe(pos, n, ip) {
        src_ip = list_entry(pos, struct fw_rule_int, list);
        if(rule->src_ip == src_ip->value) {
            list_for_each(ptr, &rules_desc_list){
                rule2 = list_entry(ptr, struct fw_rule, list);
                if(rule2->proto == rule->proto && 
                        rule2->src_ip == rule->src_ip){ 
                    exist_with_prefix = 1;
                    break;
                }
            }
            if(!exist_with_prefix) { 
                del_dst_ip_list(&src_ip->sublist);
                list_del(pos);
                kfree(src_ip);
            } else if(del_dst_ip(&src_ip->sublist, rule)){
                list_del(pos);
                kfree(src_ip);
            }
            break;
        }else if(rule->src_ip == 0) {
            rule->src_ip = src_ip->value;
            if(del_dst_ip(&src_ip->sublist, rule)){
                list_del(pos);
                kfree(src_ip);
            }
            rule->src_ip = 0;
        }
    }
    
    return list_empty(ip);
} 
 
static void del_a_rule(int num) {
    struct list_head *pos, *n;
    struct list_head *ptr;
    struct fw_rule *rule = NULL; 
    struct fw_rule *rule2; 
    struct fw_rule_char *proto;
    int exist_with_prefix = 0;
    
    list_for_each(pos, &rules_desc_list){
        rule = list_entry(pos, struct fw_rule, list);
        if(rule->id > num){
            return;
        }
        if(rule->id == num){ 
            printk(KERN_INFO "Deleting rule %u\n", num);
        
            list_del(pos);
            break;
        }
    }
    
    if(rule == NULL) return;
    
    /**Check proto**/
    list_for_each_safe(pos, n, &rules_impl_list){
        proto = list_entry(pos, struct fw_rule_char, list);
        if(rule->proto == proto->value) {
            list_for_each(ptr, &rules_desc_list){
                rule2 = list_entry(ptr, struct fw_rule, list);
                if(rule2->proto == rule->proto){ 
                    exist_with_prefix = 1;
                    break;
                }
            }
            if(!exist_with_prefix) { 
                del_src_ip_list(&proto->sublist);
                list_del(pos);
                kfree(proto);
            } else if(del_src_ip(&proto->sublist, rule)){
                list_del(pos);
                kfree(proto);
            }
            break;
        }else if(rule->proto == 0) {
            rule->proto = proto->value;
            if(del_src_ip(&proto->sublist, rule)){
                list_del(pos);
                kfree(proto);
            }
            rule->proto = 0;
        }
    } 
    
    kfree(rule);
    printk(KERN_INFO "Rule %u deleted\n", num);
    return;
}

static void add_action(struct list_head *action, struct fw_rule *rule) {
    struct list_head *pos;
    struct fw_action *act, *a;
    
    act = kmalloc(sizeof(*act), GFP_KERNEL);
    if (act==NULL) {
        printk(KERN_INFO "Error: could not allocate memory\n");
        return; 
    } 
    act->id = rule->id;
    act->action = rule->action;
    
    list_for_each(pos, action){
        a = list_entry(pos, struct fw_action, list);
        if(a->id > act->id) break;
    }
    
    list_add_tail(&act->list, pos);
}

static void copy_action(struct list_head *dst,struct list_head *src) {
    struct list_head *pos;
    struct fw_action *src_act, *dst_act;
    list_for_each(pos, src){
        src_act = list_entry(pos, struct fw_action, list);
        dst_act = kmalloc(sizeof(*dst_act), GFP_KERNEL);
        if (dst_act==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        dst_act->id = src_act->id;
        dst_act->action = src_act->action;
        list_add_tail(&dst_act->list,dst);
    }
}

static void copy_dst_port(struct list_head *dst,struct list_head *src) {
    struct list_head *pos;
    struct fw_rule_short *src_port, *dst_port;
    list_for_each(pos, src){
        src_port = list_entry(pos, struct fw_rule_short, list);
        dst_port = kmalloc(sizeof(*dst_port), GFP_KERNEL);
        if (dst_port==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        INIT_LIST_HEAD(&dst_port->sublist);
        dst_port->value = src_port->value;
        copy_action(&dst_port->sublist,&src_port->sublist);
        list_add_tail(&dst_port->list,dst);
    }
}

static void copy_src_port(struct list_head *dst,struct list_head *src) {
    struct list_head *pos;
    struct fw_rule_short *src_port, *dst_port;
    list_for_each(pos, src){
        src_port = list_entry(pos, struct fw_rule_short, list);
        dst_port = kmalloc(sizeof(*dst_port), GFP_KERNEL);
        if (dst_port==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        INIT_LIST_HEAD(&dst_port->sublist);
        dst_port->value = src_port->value;
        copy_dst_port(&dst_port->sublist,&src_port->sublist);
        list_add_tail(&dst_port->list,dst);
    }
}

static void copy_dst_ip(struct list_head *dst,struct list_head *src) {
    struct list_head *pos;
    struct fw_rule_int *src_ip, *dst_ip;
    list_for_each(pos, src){
        src_ip = list_entry(pos, struct fw_rule_int, list);
        dst_ip = kmalloc(sizeof(*dst_ip), GFP_KERNEL);
        if (dst_ip==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        INIT_LIST_HEAD(&dst_ip->sublist);
        dst_ip->value = src_ip->value;
        copy_src_port(&dst_ip->sublist,&src_ip->sublist);
        list_add_tail(&dst_ip->list,dst);
    }
}

static void copy_src_ip(struct list_head *dst,struct list_head *src) {
    struct list_head *pos;
    struct fw_rule_int *src_ip, *dst_ip;
    list_for_each(pos, src){
        src_ip = list_entry(pos, struct fw_rule_int, list);
        dst_ip = kmalloc(sizeof(*dst_ip), GFP_KERNEL);
        if (dst_ip==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        INIT_LIST_HEAD(&dst_ip->sublist);
        dst_ip->value = src_ip->value;
        copy_dst_ip(&dst_ip->sublist,&src_ip->sublist);
        list_add_tail(&dst_ip->list,dst);
    }
}

static void add_dst_port(struct list_head *port, struct fw_rule *rule) {
    struct list_head *pos;
    struct fw_rule_short *dst_port, *dp;
    int exist = 0, all = 0;
    
    list_for_each(pos, port){
        dst_port = list_entry(pos, struct fw_rule_short, list);
        if(dst_port->value == 0) all=1;
        if(rule->dst_port == dst_port->value){
            exist = 1;
            add_action(&dst_port->sublist, rule);
        }else if(rule->dst_port == 0){
            add_action(&dst_port->sublist, rule);
        }
    }
    
    if(!exist){
        dp = kmalloc(sizeof(*dp), GFP_KERNEL);
        if (dp==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        INIT_LIST_HEAD(&dp->sublist);
        dp->value = rule->dst_port;
        if(dp->value == 0) {
            list_add_tail(&dp->list,port);
            add_action(&dp->sublist,rule);
        }else{
            list_add(&dp->list,port);
            if(all) copy_action(&dp->sublist,&dst_port->sublist);
            add_action(&dp->sublist,rule);
        }
    }
}

static void add_src_port(struct list_head *port, struct fw_rule *rule) {
    struct list_head *pos;
    struct fw_rule_short *src_port, *sp;
    int exist = 0, all = 0;
    
    list_for_each(pos, port){
        src_port = list_entry(pos, struct fw_rule_short, list);
        if(src_port->value == 0) all=1;
        if(rule->src_port == src_port->value){
            exist = 1;
            add_dst_port(&src_port->sublist, rule);
        }else if(rule->src_port == 0){
            add_dst_port(&src_port->sublist, rule);
        }
    }
    
    if(!exist){
        sp = kmalloc(sizeof(*sp), GFP_KERNEL);
        if (sp==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        INIT_LIST_HEAD(&sp->sublist);
        sp->value = rule->src_port;
        if(sp->value == 0) {
            list_add_tail(&sp->list,port);
            add_dst_port(&sp->sublist,rule);
        }else{
            list_add(&sp->list,port);
            if(all) copy_dst_port(&sp->sublist,&src_port->sublist);
            add_dst_port(&sp->sublist,rule);
        }
    }
}

static void add_dst_ip(struct list_head *ip, struct fw_rule *rule) {
    struct list_head *pos;
    struct fw_rule_int *dst_ip, *di;
    int exist = 0, all = 0;
    
    list_for_each(pos, ip){
        dst_ip = list_entry(pos, struct fw_rule_int, list);
        if(dst_ip->value == 0) all = 1;
        if(rule->dst_ip == dst_ip->value){
            exist = 1;
            add_src_port(&dst_ip->sublist, rule);
        }else if(rule->dst_ip == 0){
            add_src_port(&dst_ip->sublist, rule);
        }
    }
    
    if(!exist){
        di = kmalloc(sizeof(*di), GFP_KERNEL);
        if (di==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        INIT_LIST_HEAD(&di->sublist);
        di->value = rule->dst_ip;
        if(di->value == 0) {
            list_add_tail(&di->list,ip);
            add_src_port(&di->sublist,rule);
        }else{
            list_add(&di->list,ip);
            if(all) copy_src_port(&di->sublist,&dst_ip->sublist);
            add_src_port(&di->sublist,rule);
        }
    }
}

static void add_src_ip(struct list_head *ip, struct fw_rule *rule) {
    struct list_head *pos;
    struct fw_rule_int *src_ip, *si;
    int exist = 0, all = 0;
    
    list_for_each(pos, ip){
        src_ip = list_entry(pos, struct fw_rule_int, list);
        if(src_ip->value == 0) all = 1;
        if(rule->src_ip == src_ip->value){
            exist = 1;
            add_dst_ip(&src_ip->sublist, rule);
        }else if(rule->src_ip == 0){
            add_dst_ip(&src_ip->sublist, rule);
        }
    }
    
    if(!exist){
        si = kmalloc(sizeof(*si), GFP_KERNEL);
        if (si==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        INIT_LIST_HEAD(&si->sublist);
        si->value = rule->src_ip;
        if(si->value == 0) {
            list_add_tail(&si->list,ip);
            add_dst_ip(&si->sublist,rule);
        }else{
            list_add(&si->list,ip);
            if(all) copy_dst_ip(&si->sublist,&src_ip->sublist);
            add_dst_ip(&si->sublist,rule);
        }
    }
}
    
static void add_a_rule(struct fw_rule *new_rule) {
    struct list_head *pos, *n;
    struct fw_rule *rule; 
    struct fw_rule_char *proto, *p;
    int exist = 0, all=0;
    
    list_for_each_safe(pos, n, &rules_desc_list){
        rule = list_entry(pos, struct fw_rule, list);
        if(rule->id > new_rule->id){
            break;
        }
        if(rule->id == new_rule->id){ 
            del_a_rule(rule->id);
        }
    }
    list_add_tail(&new_rule->list,pos);
    
    list_for_each(pos, &rules_impl_list){
        proto = list_entry(pos, struct fw_rule_char, list);
        if(proto->value == 0) all=1;
        if(new_rule->proto == proto->value){
            exist = 1;
            add_src_ip(&proto->sublist, new_rule);
        }else if(new_rule->proto == 0){
            add_src_ip(&proto->sublist, new_rule);
        }
    }
    
    if(!exist){
        p = kmalloc(sizeof(*p), GFP_KERNEL);
        if (p==NULL) {
          printk(KERN_INFO "Error: could not allocate memory\n");
          return; 
        }
        INIT_LIST_HEAD(&p->sublist);
        p->value = new_rule->proto;
        if(p->value == 0) {
            list_add_tail(&p->list,&rules_impl_list);
            add_src_ip(&p->sublist, new_rule);
        }else{
            list_add(&p->list,&rules_impl_list);
            if(all) copy_src_ip(&p->sublist, &proto->sublist);
            add_src_ip(&p->sublist, new_rule);
        }
    }
    
    return;
}
 
////////////////////////////////////////////////////////////////////////
///////////// communication between user aplication ////////////////////
////////////////////////////////////////////////////////////////////////
//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t rule_write( struct file *filp,const char *buf, size_t count, loff_t *f_pos)
{
    int i, value = 0;
    struct fw_rule *rule;
    
    if (count > PROCF_MAX_SIZE)
        count = PROCF_MAX_SIZE;
        
    if(copy_from_user(procf_buffer, buf, count)) return count;
    
    if(procf_buffer[0] == 'a'){
        i=1;
        rule = kmalloc(sizeof(*rule), GFP_KERNEL);
        if(rule == NULL) {
          printk(KERN_INFO "Error: could not allocate new rule\n");
          return count; 
        }
        while(i<count && procf_buffer[i]!=' ') value=value*10 + (procf_buffer[i++]-'0');
        rule->id = value;
        value = 0;
        i++;
        
        while(i<count && procf_buffer[i]!=' ') value=value*10 + (procf_buffer[i++]-'0');
        rule->proto = value;
        value = 0;
        i++;
        
        while(i<count && procf_buffer[i]!=' ') value=value*10 + (procf_buffer[i++]-'0');
        rule->src_ip = value;
        value = 0;
        i++;
        
        while(i<count && procf_buffer[i]!=' ') value=value*10 + (procf_buffer[i++]-'0');
        rule->dst_ip = value;
        value = 0;
        i++;
        
        while(i<count && procf_buffer[i]!=' ') value=value*10 + (procf_buffer[i++]-'0');
        rule->src_port = value;
        value = 0;
        i++;
        
        while(i<count && procf_buffer[i]!=' ') value=value*10 + (procf_buffer[i++]-'0');
        rule->dst_port = value;
        value = 0;
        i++;
        
        while(i<count && procf_buffer[i]!=' ') value=value*10 + (procf_buffer[i++]-'0');
        rule->action = value;
        value = 0;
        i++;
        
        add_a_rule(rule);
    }else if(procf_buffer[0] == 'd'){
        for(i=1;i<count;++i) value=value*10 + (procf_buffer[i]-'0');
        del_a_rule(value);
    }
    
    return count;
}

static void *rule_seq_start(struct seq_file *s, loff_t *pos)
{
    int i=0;
    struct list_head *p;

    printk(KERN_INFO "rule_seq_start is called.\n");

    list_for_each(p, &rules_desc_list) {
        if(i++ == *pos)
            return p;
    }
    return NULL;
}

static void *rule_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    printk(KERN_INFO "rule_seq_next is called.\n");
    (*pos)++;
    if (((struct list_head *)v)->next == &rules_desc_list)
        return NULL;
    return ((struct list_head *)v)->next;
}

static int rule_seq_show(struct seq_file *s, void *v)
{
    struct fw_rule *p = list_entry(v, struct fw_rule, list);

    printk(KERN_INFO "rule_seq_show is called.\n");
    seq_printf(s, "%u\t%s\t", p->id, (p->action == NF_ACCEPT)?"allow":"deny");
    if(p->src_ip == 0) seq_printf(s,"*\t");
    else seq_printf(s,"%u.%u.%u.%u\t",
        (unsigned char)(p->src_ip >> 24),
        (unsigned char)(p->src_ip >> 16),
        (unsigned char)(p->src_ip >> 8),
        (unsigned char)(p->src_ip)
    ); 
    if(p->src_port == 0) seq_printf(s,"*\t");
    else seq_printf(s,"%u\t", p->src_port);
    
    if(p->dst_ip == 0) seq_printf(s,"*\t");
    else seq_printf(s,"%u.%u.%u.%u\t",
        (unsigned char)(p->dst_ip >> 24),
        (unsigned char)(p->dst_ip >> 16),
        (unsigned char)(p->dst_ip >> 8),
        (unsigned char)(p->dst_ip)
    ); 
    if(p->dst_port == 0) seq_printf(s,"*\t");
    else seq_printf(s,"%u\t", p->dst_port);
    
    seq_printf(s, "%s\n",
        (p->proto == IPPROTO_TCP)?"tcp":
        (p->proto == IPPROTO_UDP)?"udp":
        (p->proto == IPPROTO_ICMP)?"icmp":
        "ip"
    );
    return 0;
}

static void rule_seq_stop(struct seq_file *s, void *v){}

static struct seq_operations rule_seq_ops = {
	.start = rule_seq_start,
	.next  = rule_seq_next,
	.stop  = rule_seq_stop,
	.show  = rule_seq_show
};

static int rule_open(struct inode *inode, struct file *file)
{
  return seq_open(file, &rule_seq_ops);
};

static struct file_operations rule_file_ops = {
  .owner   = THIS_MODULE,
  .open    = rule_open,
  .read    = seq_read,
  .write   = rule_write,
  .llseek  = seq_lseek,
  .release = seq_release
};

////////////////////////////////////////////////////////////////////////
////////// end of communication between user aplication ////////////////
////////////////////////////////////////////////////////////////////////

 
/* Initialization routine */
int init_module() {
    printk(KERN_INFO "initialize kernel module\n");
    /**create proc file**/
    //proc_file = create_proc_entry(PROCF_NAME, 0644, NULL);
    proc_file = proc_create(PROCF_NAME, 0644, NULL, &rule_file_ops);
    if (proc_file==NULL) {
        printk(KERN_INFO "Error: could not initialize /proc/%s\n", PROCF_NAME);
        return -ENOMEM; 
    } 
    procf_buffer = (char *) vmalloc(PROCF_MAX_SIZE);
    if (procf_buffer==NULL) {
        printk(KERN_INFO "Error: could not allocate proc buffer\n");
        return -ENOMEM; 
    } 
    //proc_file->proc_fops = &rule_file_ops;
    printk(KERN_INFO "/proc/%s is created\n", PROCF_NAME);
    
    /**register input filter function**/
    nfho.hook     = filter_in;
    nfho.pf       = PF_INET;
    nfho.hooknum  = NF_INET_PRE_ROUTING;
    nfho.priority = NF_IP_PRI_FIRST;
    nf_register_hook(&nfho);
    return 0;
}
 
/* Cleanup routine */
void cleanup_module() {
    struct list_head *pos,*n;
    struct fw_rule *rule; 
    
    /**unregister input filter function**/
    nf_unregister_hook(&nfho);
    /**remove proc entry**/
    remove_proc_entry(PROCF_NAME, NULL);
    vfree(procf_buffer);
    
    list_for_each_safe(pos, n, &rules_desc_list){
        rule = list_entry(pos, struct fw_rule, list);
        kfree(rule);
    }
    del_proto_list(&rules_impl_list);
    printk(KERN_INFO "kernel module unloaded.\n");
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
 
