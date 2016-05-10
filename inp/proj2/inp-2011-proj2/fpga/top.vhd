-- top.vhd : TOP level entity, BrainFuck microcontroller
-- Copyright (C) 2011 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Zdenek Vasicek
--

library IEEE;
use IEEE.std_logic_1164.ALL;
use IEEE.std_logic_ARITH.ALL;
use IEEE.std_logic_UNSIGNED.ALL;

architecture main of tlv_bare_ifc is 
   -- displej
   signal lcd_data  : std_logic_vector(7 downto 0);
   signal lcd_we, lcd_busy    : std_logic;

	signal rom_addr : std_logic_vector(11 downto 0);
	signal rom_dout : std_logic_vector(7 downto 0);
	signal rom_en   : std_logic;

   signal ram_addr: std_logic_vector(9 downto 0);
	signal ram_rdata: std_logic_vector(7 downto 0);
	signal ram_wdata: std_logic_vector(7 downto 0);
   signal ram_rdwr: std_logic;
	signal ram_en   : std_logic;

   signal in_data : std_logic_vector(7 downto 0);
   signal in_req : std_logic;
   signal kb_ack : std_logic;
   signal in_vld : std_logic := '0';
   
   component cpu is
    port (
      RESET : in std_logic;
      CLK   : in std_logic;
      EN    : in std_logic;
    
      -- ROM mem
      CODE_ADDR : out std_logic_vector(11 downto 0);
      CODE_DATA : in std_logic_vector(7 downto 0);
      CODE_EN   : out std_logic;
      
      -- RAM mem
      DATA_ADDR  : out std_logic_vector(9 downto 0);
      DATA_WDATA : out std_logic_vector(7 downto 0);
      DATA_RDATA : in std_logic_vector(7 downto 0);
      DATA_RDWR  : out std_logic;
      DATA_EN    : out std_logic;
      
      -- INPUT
      IN_DATA   : in std_logic_vector(7 downto 0);
      IN_VLD    : in std_logic;
      IN_REQ    : out std_logic;
      
      -- OUTPUT
      OUT_DATA : out  std_logic_vector(7 downto 0);
      OUT_BUSY : in std_logic;
      OUT_WE   : out std_logic
    );
   end component;  

begin
   -- ================================================
   -- Procesor
   -- ================================================
   bfcpu: cpu
   port map(
      --Ridici signaly
      RESET => RESET, -- Reset
      CLK   => CLK,   -- Hodiny
      EN    => '1',   -- povoleni cinnosti

      --ROM
      CODE_ADDR => rom_addr,
      CODE_DATA => rom_dout,
      CODE_EN   => rom_en,

      --RAM
	   DATA_ADDR => ram_addr,
	   DATA_RDATA => ram_rdata,
	   DATA_WDATA => ram_wdata,
	   DATA_RDWR => ram_rdwr,
	   DATA_EN   => ram_en,
		
		--INPUT
	   IN_DATA   => in_data,
  	   IN_VLD    => in_vld,
	   IN_REQ    => in_req,
		
		--OUTPUT
      OUT_DATA  => lcd_data,
      OUT_BUSY  => lcd_busy,
      OUT_WE    => lcd_we
   );
		
   -- ================================================
   -- Pamet programu
   -- ================================================
	rom_mem: entity work.rom
   generic map (
      -- Inicializace obsahu pameti ROM (pamet programu obsahujici kod v jazyce BrainFuck),
      -- odkomentovat vzdy pouze jeden radek nastavujici generickou promennou INIT

      -- Vyuziti programu pro overeni cinnosti:
      --  [1] - overeni vystupu
      --  [2], [5] - overeni jednoduche while smycky a vystupu
      --  [3] - overeni vstupu a vystupu
      --  [4] - overeni vstupu a vystupu, vlozene komentare
      --  [6], [7], [8] - overeni korektni funkce vnorenych smycek, vstupu a vystupu

      -- [1] Vypis textu na displej 
      INIT => "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.+.+.+.+.+.+.+." & nul

      -- [2] Vypis textu na displej 
      -- INIT => "++++++++++[>+++++++>+++>+++++<<<-]>+++.+++++.++.>>-----.+++++.--.+..----.<<++++++++++++++++++++++++++++++++.++.---.-----.>>+++++." & nul

      -- [3] Vypis znaku stisknutych na klavesnici na displej (vypise se max. 10 znaku, pote se program ukonci) 
      -- INIT => "++++++++++[>,.<-] 10x opakovat nacteni znaku a zapis na display" & nul

      -- [4] Vypis znaku stisknutych na klavesnici na displej (vypisuje se dokud neni stisknuta klavesa * nebo # (tzn. konec radku))
      -- INIT => "+[->,[>+>+<<-] nacteni vstupu do bunky 1 kopirovani nactene hodnoty do bunky 2 a 3 +>---------- odecteni 10 od bunky 2 [>.<[-]<<+>>>[-]<  pokud byla hodnota bunky 2 rozdilna od 0 vypis na display znulovani bunky 2 znulovani bunky 3]<<]" & nul

      -- [5] Vypis mocnin dvou od 0 do 10000
      -- INIT => "++++[>+++++<-]>[<+++++>-]+<+[    >[>+>+<<-]++>>[<<+>>-]>>>[-]++>[-]+    >>>+[[-]++++++>>>]<<<[[<++++++++<++>>-]+<.<[>----<-]<]    <<[>>>>>[>>>[-]+++++++++<[>-<-]+++++++++>[-[<->-]+[<<<]]<[>+<-]>]<<-]<<-][Outputs square numbers from 0 to 10000.Daniel B Cristofani (cristofdathevanetdotcom)http://www.hevanet.com/cristofd/brainfuck/]" & nul

      -- [6] Vypis prvocisel (vypis prvocisel na displej az do maxima zadaneho na klavesnici; postup: zadat cislo, potvrdit klavesou #)
      --	INIT => "+[->,----------[<+>-------------------------------------->[>+>+<<-]>>[<<+>>-]<>>>+++++++++[<<<[>+>+<<-]>>[<<+>>-]<[<<+>>-]>>-]<<<[-]<<[>+<-]]<]>>[<<+>>-]<<>+<-[>+[>+>+<<-]>>[<<+>>-]<>+<-->>>>>>>>+<<<<<<<<[>+<-<[>>>+>+<<<<-]>>>>[<<<<+>>>>-]<<<>[>>+>+<<<-]>>>[<<<+>>>-]<<<<>>>[>+>+<<-]>>[<<+>>-]<<<[>>>>>+<<<[>+>+<<-]>>[<<+>>-]<[>>[-]<<-]>>[<<<<[>+>+<<-]>>[<<+>>-]<>>>-]<<<-<<-]+>>[<<[-]>>-]<<>[-]<[>>>>>>[-]<<<<<<-]<<>>[-]>[-]<<<]>>>>>>>>[-<<<<<<<[-]<<[>>+>+<<<-]>>>[<<<+>>>-]<<<>>[>+<-]>[[>+>+<<-]>>[<<+>>-]<>+++++++++<[>>>+<<[>+>[-]<<-]>[<+>-]>[<<++++++++++>>-]<<-<-]+++++++++>[<->-]<[>+<-]<[>+<-]<[>+<-]>>>[<<<+>>>-]<>+++++++++<[>>>+<<[>+>[-]<<-]>[<+>-]>[<<++++++++++>>>+<-]<<-<-]>>>>[<<<<+>>>>-]<<<<>[-]<<+>]<[[>+<-]+++++++[<+++++++>-]<-><.[-]>>[<<+>>-]<<-]>++++[<++++++++>-]<.[-]>>>>>>>]<<<<<<<<>[-]<[-]<<-]++++++++++.[-]" & nul

      -- [7] Faktorizace cisla (rozklad cisla zadaneho na klavesnici na prvocisla a zobrazeni na displej; postup: zadat cislo, potvrdit klavesou #; overeni viz http://www.numberempire.com/numberfactorizer.php)
      -- INIT => ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-<<<<<<<<<+[-[>>>>>>>>>>][-]<<<<<<<<<<[[->>>>>>>>>>+<<<<<<<<<<]<<<<<<<<<<]>>>>>>>>>>,----------]>>>>>>>>>>[------------------------------------->>>>>>>>>->]<[+>[>>>>>>>>>+>]<-<<<<<<<<<<]-[>++++++++++++++++++++++++++++++++++++++++++++++++.------------------------------------------------<<<<<<<<<<<]++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.--------------------------.[-]>>>>>>>>>>>>++<<<<+[[-]>>[>>>>[-]>[-]>[-]>[-]>[-]>[-]<<<<<<<[->>>+>+<<<<]>>>>>>>>]<<<<<<<<<<[>>>>>>[-<<<<+>>>>]<<<<<<<<<<<<<<<<]>>>>>>>>>>[>[->>>+>>+<<<<<]>>>>>>>>>]<<<<<<<<<<[>>>>>>[-<<<<<+>>>>>]<<<<<<<<<<<<<<<<]>>>>>>>>>>[>>>[-]>>>[-]>[-]>>>]<<<<<<<<<<[<<<<<<<<<<]>>>>>>>>>[-]>>>>>>>+<<<<<<<<[+]+[->>[>>>>>>[->++<]>>>>]<<<<<<<<<<[>>>>>>>>[-]>[-]<<<<[->>>++<<<]<<<<<<<<<<<<<<<]>>>>>>>>>>[>>>>>>>>[->+<[->+<[->+<[->+<[->+<[->+<[->+<[->+<[->+<[->--------->>>>>>>>>+<<<<<<<<<<[->+<]]]]]]]]]]]>>]<<<<<<<<<<[>>>>>>>>>[-<+<<<+>>>>]<<<<<<<<<<<<<<<<<<<]>>>>>>>>>>[>>>>>>>[-<+>[-<+>[-<+>[-<+>[-<+>[-<+>[-<+>[-<+>[-<+>[-<--------->>>>>>>>>>>+<<<<<<<<<<[-<+>]]]]]]]]]]]>>>]<<<<<<<<<<[>>>>[->>>+>>+<<<<<]<<<<<<<<<<<<<<]>>>>>>>>>>[>>>>>>>[-<<<+>>>]>>>]<<<<<<<<<<[>>>>>>>>[->-<]>[<<<<<<<<<[<[-]>>>>>>>>>>[-<<<<<<<<<<+>>>>>>>>>>]<<<<<<<<<<<<<<<<<<<]>>>>>>>>>>>>>>>>>>>]<<<<<<<<<<<<<<<<<<<]>>>>>>>>>[+[+[+[+[+[+[+[+[+[+[[-]<+>]]]]]]]]]]]<]>>>>>>>>[<<<<<<[>>>>>>>>[-]>[-]<<<<<[->>>+>+<<<<]>>>>>>]<<<<<<<<<<[>>>>>>>>[-<<<<+>>>>]<<<[->>>+>+<<<<]<<<<<<<<<<<<<<<]>>>>>>>>>>[>>>>>>>>>[-<<<<+>>>>]>]<<<<<<<<<<[>>>>>>>>[-<->]<<<<<<<<<<<<<<<<<<]>>>>>>>>>>[>>>>>>>[->+<[->+<[->+<[->+<[->+<[->+<[->+<[->+<[->+<[->+<[++++++++++[+>-<]>>>>>>>>>>-<<<<<<<<<<]]]]]]]]]]]>>>]>>>>>>>+[[-]<<<<<<<<<<<<<<<<<[>>>>[-]>>>>[-<<<<+>>>>]<<[->>+<<]<<<<<<<<<<<<<<<<]>>>>>>>>>>[>>>>>>>>[->+<<<+>>]>>]<<<<<<<<<<[>>>[->>>>>>+<<<<<<]<<<<<<<<<<<<<]>>>>>>>>>>[>>>>>>>>>[-<<<<<<+>>>>>>[-<<<<<<+>>>>>>[-<<<<<<+>>>>>>[-<<<<<<+>>>>>>[-<<<<<<+>>>>>>[-<<<<<<+>>>>>>[-<<<<<<+>>>>>>[-<<<<<<+>>>>>>[-<<<<<<+>>>>>>[-<<<<<<--------->>>>>>>>>>>>>>>>+<<<<<<<<<<[-<<<<<<+>>>>>>]]]]]]]]]]]>]>>>>>>>]<<<<<<<<<<<<<<<<<[<<<<<<<<<<]>>>>>>>>>>[>>>>>>>>[-]<<[->+<]<[->>>+<<<]>>>>>]<<<<<<<<<<[+>>>>>>>[-<<<<<<<+>>>>>>>[-<<<<<<<->>>>>>+>[-<<<<<<<+>>>>>>>[-<<<<<<<->>>>>>+>[-<<<<<<<+>>>>>>>[-<<<<<<<->>>>>>+>[-<<<<<<<+>>>>>>>[-<<<<<<<->>>>>>+>[-<<<<<<<+>>>>>>>]]]]]]]]]<<<<<<<[->>>>>>>+<<<<<<<]-<<<<<<<<<<]>>>>>>>[-<<<<<<<<<<<+>>>>>>>>>>>]>>>[>>>>>>>[-<<<<<<<<<<<+++++>>>>>>>>>>>]>>>]<<<<<<<<<<[+>>>>>>>>[-<<<<<<<<+>>>>>>>>[-<<<<<<<<->>>>>+>>>[-<<<<<<<<+>>>>>>>>[-<<<<<<<<->>>>>+>>>[-<<<<<<<<+>>>>>>>>[-<<<<<<<<->>>>>+>>>[-<<<<<<<<+>>>>>>>>[-<<<<<<<<->>>>>+>>>[-<<<<<<<<+>>>>>>>>]]]]]]]]]<<<<<<<<[->>>>>>>>+<<<<<<<<]-<<<<<<<<<<]>>>>>>>>[-<<<<<<<<<<<<<+>>>>>>>>>>>>>]>>[>>>>>>>>[-<<<<<<<<<<<<<+++++>>>>>>>>>>>>>]>>]<<<<<<<<<<[<<<<<<<<<<]>>>>>>>>>>>>>>>>]<<<<<<[>>>[->>>>+>+<<<<<]>>>>>>>]<<<<<<<<<<[>>>>>>>[-<<<<+>>>>]<<<<<[->>>>>+>>+<<<<<<<]<<<<<<<<<<<<]>>>>>>>>>>[>>>>>>>[-<<<<<+>>>>>]>>>]<<<<<<<<<<[>>>>>>>>>[-<->]<[<<<<<<<<[<<[-]>>>>>>>>>>[-<<<<<<<<<<+>>>>>>>>>>]<<<<<<<<<<<<<<<<<<]>>>>>>>>>>>>>>>>>>]<<<<<<<<<<<<<<<<<<]>>>>>>>>[>-<[+[+[+[+[+[+[+[+[+[[-]>+<]]]]]]]]]]]>+[[-]<[-]+>>>>+>>>>>>>>[>>>>>>>>>>]<<<<<<<<<<[<<<<<<[<<<<[<<<<<<<<<<]>>>>+<<<<<<<<<<]<<<<]>>>>>>>>>>>>>>>>>>>>[>>>>>>>>>>]<<<<<<<<<<[<<<<<<<<<<]>>>>-[[+]>>>>>>>>-<<[>[-]>>[-<<+>>]>>>>>>>]<<<<<<<<<<[+>>[>>>>>>>>+>>]<<-<<<<<<<<<<]-[>>++++++++++++++++++++++++++++++++++++++++++++++++.------------------------------------------------<<<<<<<<<<<<]++++++++++++++++++++++++++++++++.[-]>>>>]>>>>>>[>>[->>>>>+<<<<<[->>>>>+<<<<<[->>>>>+<<<<<[->>>>>+<<<<<[->>>>>+<<<<<[->>>>>+<<<<<[->>>>>+<<<<<[->>>>>+<<<<<[->>>>>+<<<<<[->>>>>--------->>>>>+<<<<<<<<<<[->>>>>+<<<<<]]]]]]]]]]]>>>>>>>>]<<<<<<<<<<[>>>>>>>[-<<<<<+>>>>>]<<<<<<<<<<<<<<<<<]>>>>>>>>>]<]>>[>>>>>>>>>>]<<<<<<<<<<[+>[>>>>>>>>>+>]<-<<<<<<<<<<]-[>++++++++++++++++++++++++++++++++++++++++++++++++.<<<<<<<<<<<]++++++++++." & nul

      -- [8] Prevod binarniho cisla na ASCII (ocekava na klavesnici zadane 8-bitove binarni cisla, tj. obsahujici znaky 0 / 1)
      -- INIT => ">,[>>>++++++++[<[<++>-]<+[>+<-]<-[-[-<]>]>[-<]<,>>>-]<.[-]<<]"
   )
   port map (
      CLK => CLK, 
		EN => rom_en,
		ADDR => rom_addr,
		DATA => rom_dout
   );
	
   -- ================================================
   -- Pamet dat
   -- ================================================
   ram_mem: entity work.ram
   port map(
      CLK   => CLK,
      -- RAM
      ADDR  => ram_addr,
      RDATA => ram_rdata,
      WDATA => ram_wdata,
      EN    => ram_en,
      RDWR  => ram_rdwr
   );

   -- ================================================
   -- Radic LCD displeje
   -- ================================================
   lcd_u : entity work.lcd_controller
   generic map (
      -- pragma translate off
      -- Pouze pro ucely simulace se zkrati trvani prikazu zapisu na displej.
      -- Pri synteze se pouzije defaultni hodnota nastavena v radici.
      CMDLEN => 1000,
      -- pragma translate on

      LCD2x16 => True -- pro FITkit s dvouradkovym displejem nastavit na True, jinak False
   )
   port map(
      CLK         => CLK,
      RST         => RESET,
   
      -- user interface
      DATA_IN     => lcd_data,
      WRITE_EN    => lcd_we,
   	BUSY        => lcd_busy,
   
      -- lcd interface
      DISPLAY_RS    => LRS,
      DISPLAY_RW    => LRW,
      DISPLAY_EN    => LE,
      DISPLAY_DATA  => LD
   );

   -- ================================================
   -- Radic klavesnice
   -- ================================================
   kb_u : entity work.kb_controller
   port map(
      CLK         => CLK,
      RST         => RESET,
   
      -- user interface
      DATA        => in_data,
      DATA_VLD    => in_vld,
   	DATA_REQ    => in_req,
      REQ_ACK     => kb_ack,
   
      -- lcd interface
      KB_IN    => KIN,
      KB_OUT    => KOUT
   );

   --pokud se ceka na stisk tlacitka, rozsvitit diodu D4
   LEDF <= not kb_ack; 

end main;
