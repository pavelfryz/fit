-- fsm.vhd: Finite State Machine
-- Author(s): Pavel Frýz, xfryzp00@stud.fit.vutbr.cz
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (S1, S2, S3, S4, S5, S6A, S6B,
   S7A, S7B, S8A, S8B, S9A, S9B, S10, GRANTED, DENIED, 
   PRINT_ACCESS_GRANTED, PRINT_ACCESS_DENIED, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= S1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- Pocatecni stav, zadny kod nezadan
   when S1 =>
      next_state <= S1;
      if (KEY(2) = '1') then
         next_state <= S2;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:2
   when S2 =>
      next_state <= S2;
      if (KEY(7) = '1') then
         next_state <= S3;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:27
   when S3 =>
      next_state <= S3;
      if (KEY(6) = '1') then
         next_state <= S4;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:276
   when S4 =>
      next_state <= S4;
      if (KEY(3) = '1') then
         next_state <= S5;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:2763
   when S5 =>
      next_state <= S5;
      if (KEY(2) = '1') then
         next_state <= S6A;
      elsif (KEY(0) = '1') then
         next_state <= S6B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:27632
   when S6A =>
      next_state <= S6A;
      if (KEY(3) = '1') then
         next_state <= S7A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:27630
   when S6B =>
      next_state <= S6B;
      if (KEY(4) = '1') then
         next_state <= S7B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:276323
   when S7A =>
      next_state <= S7A;
      if (KEY(4) = '1') then
         next_state <= S8A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:276304
   when S7B =>
      next_state <= S7B;
      if (KEY(2) = '1') then
         next_state <= S8B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:2763234
   when S8A =>
      next_state <= S8A;
      if (KEY(1) = '1') then
         next_state <= S9A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:2763042
   when S8B =>
      next_state <= S8B;
      if (KEY(6) = '1') then
         next_state <= S9B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;       
   -- Zadano:27632341
   when S9A =>
      next_state <= S9A;
      if (KEY(1) = '1') then
         next_state <= S10;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:27630426
   when S9B =>
      next_state <= S9B;
      if (KEY(4) = '1') then
         next_state <= S10;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:276323411 nebo 276304264
   when S10 =>
      next_state <= S10;
      if (KEY(8) = '1') then
         next_state <= GRANTED;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;
   -- Zadano:2763234118 nebo 2763042648
   when GRANTED =>
      next_state <= GRANTED;
      if (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_GRANTED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= DENIED;
      end if;                       
   -- Zadan spatny kod
   when DENIED =>
      next_state <= DENIED;
      if (KEY(15) = '1') then
         next_state <= PRINT_ACCESS_DENIED; 
      end if;
   -- Tisknuti "Pristup povolen"
   when PRINT_ACCESS_GRANTED =>
      next_state <= PRINT_ACCESS_GRANTED;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- Tisknuti "Pristup odepren"
   when PRINT_ACCESS_DENIED =>
      next_state <= PRINT_ACCESS_DENIED;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- Cekani na stisk klavesy '#'
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= S1; 
      end if;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ACCESS_GRANTED =>
      FSM_MX_MEM     <= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ACCESS_DENIED =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

