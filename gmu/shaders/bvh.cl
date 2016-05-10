int shift2(int x){
  x = (x ^ (x << 16)) & 0xff0000ff;
  x = (x ^ (x <<  8)) & 0x0300f00f; 
  x = (x ^ (x <<  4)) & 0x030c30c3; 
  x = (x ^ (x <<  2)) & 0x09249249; 
  return x;
}

__kernel void calcMortons(__global float *centers, __global int *mortons, int count, float worldminx, float worldminy, float worldminz, float scalex, float scaley, float scalez)
{
  int id = (int)get_global_id(0);
  if(id < count){
    int x = (centers[3*id]-worldminx)*scalex; 
    int y = (centers[3*id+1]-worldminy)*scaley; 
    int z = (centers[3*id+2]-worldminz)*scalez; 
    if(x>=1024) x = 1023;
    if(y>=1024) y = 1023;
    if(z>=1024) z = 1023;
    x = shift2(x);
    y = shift2(y);
    z = shift2(z);
    mortons[id] = (x<<2) + (y<<1) + z;
  }
}

__kernel void generateHierarchy(__global int *indexes, __global int *mortons, __global int *tree, int count) {
  int id = (int)get_global_id(0);
  int zero1;
  int zero2;
  int dir;
  int dMin;
  int lMax = 1;
  int tmp;
  int mId;
  
  if(id < count){
    mId=mortons[indexes[id]];
    zero1 = (id+1>count)?-1:clz(mId^mortons[indexes[id+1]]);
    zero2 = (id-1<0)?-1:clz(mId^mortons[indexes[id-1]]);
    if(zero1==32) zero1+=clz(id^(id+1));
    if(zero2==32) zero1+=clz(id^(id-1));
    int dir=(zero1-zero2>=0) ? 1 : -1;

    if(zero1-zero2>=0){
      dir=1;
      dMin=zero2;
    }else{
      dir=-1;
      dMin=zero1;
    }
    
    do{
      lMax *=2;
      tmp = (id+lMax*dir<0||id+lMax*dir>count)?-1:clz(mId^mortons[indexes[id+lMax*dir]]);
      if(tmp==32) tmp+=clz(id^(id+lMax*dir));
    }while(tmp>dMin);

    int l = 0;
    for(int t=lMax/2;t>0;t/=2){
      tmp = (id+(l+t)*dir<0||id+(l+t)*dir>count)?-1:clz(mId^mortons[indexes[id+(l+t)*dir]]);
      if(tmp==32) tmp+=clz(id^(id+(l+t)*dir));
      if(tmp>dMin) l+=t;
    }
    int j = id+l*dir;  

    int first = min(id,j);
    int last = max(id,j);
    int commonPrefix = clz(mortons[indexes[first]]^mortons[indexes[last]]);
    if (commonPrefix==32) commonPrefix+=clz(first^last);
    int split = first;
    int s = last-first;
    do{
      s=(s+1)/2;
      if(split+s<last){
        tmp = clz(mortons[indexes[first]]^mortons[indexes[split+s]]);
        if(tmp==32) tmp+=clz(first^split+s);
        if(tmp>commonPrefix) split+=s;
      }
    }while(s>1);
    
    int left,right;
    if(first==split) left = -split-1; else left = split;
    if(last==split+1) right = -split-2; else right = split+1;
    tree[2*id]=left;
    tree[2*id+1]=right;
  }
}
