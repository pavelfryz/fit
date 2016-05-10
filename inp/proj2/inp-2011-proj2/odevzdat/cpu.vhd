-- cpu.vhd - Jednoduchý procesor
-- Author: Pavel Frýz <xfryzp00@stud.fit.vutbr.cz>
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru

   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti

   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti

   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data

   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;



-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
type fsm_state is (sidle,sfetch,sdecode,sincptr,sdecptr,sinc1,sinc2,
  sdec1,sdec2,sread1,sread2,swrite1,swrite2,sjmpz1,sjmpz2,sjmpz3,sjmpz4,
  sjmpnz1,sjmpnz2,sjmpnz3,sjmpnz4,snop,shalt);

signal nstate: fsm_state;
signal pstate: fsm_state;

signal pc_reg : std_logic_vector(11 downto 0);
signal pc_inc : std_logic;
signal pc_dec : std_logic;

signal ptr_reg : std_logic_vector(9 downto 0);
signal ptr_inc : std_logic;
signal ptr_dec : std_logic;

signal cnt_reg : std_logic_vector(7 downto 0);
signal cnt_inc : std_logic;
signal cnt_dec : std_logic;

signal cnt_zero : std_logic;
signal data_zero : std_logic;

begin
  CODE_ADDR <= pc_reg;
  DATA_ADDR <= ptr_reg;

  cnt_zero <= '1' when cnt_reg=(cnt_reg'range => '0') else '0';
  data_zero <= '1' when DATA_RDATA=(DATA_RDATA'range => '0') else '0';
  -- Program counter (PC)
  pc_cntr: process(RESET, CLK)
  begin
    if (RESET='1') then
      pc_reg <= (others=>'0');
    elsif (CLK'event) and (CLK='1') then
      if (pc_inc='1') then
        pc_reg <= pc_reg + 1;
      elsif (pc_dec='1') then
        pc_reg <= pc_reg - 1;
      end if;
    end if;
  end process;
  -- Adress pointer (PTR)
  ptr_cntr: process(RESET, CLK)
  begin
    if (RESET='1') then
      ptr_reg <= (others=>'0');
    elsif (CLK'event) and (CLK='1') then
      if (ptr_inc='1') then
        ptr_reg <= ptr_reg + 1;
      elsif (ptr_dec='1') then
        ptr_reg <= ptr_reg - 1;
      end if;
    end if;
  end process;
  -- Counter (CNT)
  cnt_cntr: process(RESET, CLK)
  begin
    if (RESET='1') then
      cnt_reg <= (others=>'0');
    elsif (CLK'event) and (CLK='1') then
      if (cnt_inc='1') then
        cnt_reg <= cnt_reg + 1;
      elsif (cnt_dec='1') then
        cnt_reg <= cnt_reg - 1;
      end if;
    end if;
  end process;
  -- FSM present state
  fsm_pstate: process(RESET, CLK)
  begin
    if (RESET='1') then
      pstate <= sidle;
    elsif (CLK'event) and (CLK='1') then
      if (EN = '1') then
        pstate <= nstate;
      end if;
    end if;
  end process;
  -- FSM next state logic
  nsl: process(pstate,IN_VLD,OUT_BUSY,CODE_DATA,DATA_RDATA,IN_DATA,data_zero,cnt_zero)
  begin
    nstate <= sidle;
    OUT_DATA <= (others=>'0');
    DATA_WDATA <= (others=>'0');

    CODE_EN <= '0';
    DATA_EN <= '0';
    DATA_RDWR <= '0';
    IN_REQ <= '0';
    OUT_WE <= '0';

    pc_inc <= '0';
    pc_dec <= '0';
    ptr_inc <= '0';
    ptr_dec <= '0';
    cnt_inc <= '0';
    cnt_dec <= '0';

    case pstate is
      -- IDLE
      when sidle =>
        nstate <= sfetch;
      -- INSTRUCTION FETCH
      when sfetch =>
        nstate <= sdecode;
        CODE_EN <= '1';
      -- INSTRUCTION DECODE
      when sdecode =>
        case CODE_DATA is
          when x"3E" => nstate <= sincptr;
          when x"3C" => nstate <= sdecptr;
          when x"2B" => nstate <= sinc1;
          when x"2D" => nstate <= sdec1;
          when x"5B" => nstate <= sjmpz1;
          when x"5D" => nstate <= sjmpnz1;
          when x"2E" => nstate <= swrite1;
          when x"2C" => nstate <= sread1;
          when x"00" => nstate <= shalt;
          when others => nstate <= snop;
        end case;
      -- INCPTR
      when sincptr =>
        nstate <= sfetch;
        ptr_inc <= '1';
        pc_inc <= '1';
      -- DECPTR
      when sdecptr =>
        nstate <= sfetch;
        ptr_dec <= '1';
        pc_inc <= '1';
      -- INC
      when sinc1 =>
        nstate <= sinc2;
        DATA_EN <= '1';
      when sinc2 =>
        nstate <= sfetch;
        DATA_WDATA <= DATA_RDATA + 1;
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        pc_inc <= '1';
      -- DEC
      when sdec1 =>
        nstate <= sdec2;
        DATA_EN <= '1';
      when sdec2 =>
        nstate <= sfetch;
        DATA_WDATA <= DATA_RDATA - 1;
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        pc_inc <= '1';
      -- READ
      when sread1 =>
        nstate <= sread2;
        IN_REQ <= '1';
      when sread2 =>
        if (IN_VLD='1') then
          nstate <= sfetch;
          DATA_WDATA <= IN_DATA;
          DATA_EN <= '1';
          DATA_RDWR <= '1';
          pc_inc <= '1';
        else
          nstate <= sread2;
        end if;
      -- WRITE
      when swrite1 =>
        nstate <= swrite2;
        DATA_EN <= '1';
      when swrite2 =>
        if (OUT_BUSY='1') then
          nstate <= swrite2;
        else
          nstate <= sfetch;
          OUT_DATA <= DATA_RDATA;
          OUT_WE <= '1';
          pc_inc <= '1';
        end if;
      -- JMPZ
      when sjmpz1 =>
        nstate <= sjmpz2;
        DATA_EN <= '1';
      when sjmpz2 =>
        pc_inc <= '1';
        if (data_zero = '1') then
          nstate <= sjmpz3;
        else
          nstate <= sfetch;
        end if;
      when sjmpz3 =>
        nstate <= sjmpz4;
        CODE_EN <= '1';
      when sjmpz4 =>
        nstate <= sjmpz3;
        pc_inc <= '1';
        if (CODE_DATA = x"5B") then
          cnt_inc <= '1';
        elsif (CODE_DATA = x"5D") then
          if (cnt_zero = '1') then
            nstate <= sfetch;
          else
            cnt_dec <= '1';
          end if;
        end if;
      -- JMPNZ
      when sjmpnz1 =>
        nstate <= sjmpnz2;
        DATA_EN <= '1';
      when sjmpnz2 =>
        if (data_zero = '1') then
          pc_inc <= '1';
          nstate <= sfetch;
        else
          pc_dec <= '1';
          nstate <= sjmpnz3;
        end if;
      when sjmpnz3 =>
        nstate <= sjmpnz4;
        CODE_EN <= '1';
      when sjmpnz4 =>
        nstate <= sjmpnz3;
        pc_dec <= '1';
        if (CODE_DATA = x"5B") then
          if (cnt_zero = '1') then
            pc_dec <= '0';
            pc_inc <= '1';
            nstate <= sfetch;
          else
            cnt_dec <= '1';
          end if;
        elsif (CODE_DATA = x"5D") then
          cnt_inc <= '1';
        end if;
      -- NOP
      when snop =>
        nstate <= sfetch;
        pc_inc <= '1';
      -- HALT
      when shalt =>
        nstate <= shalt;
    end case;
  end process;
end behavioral;
