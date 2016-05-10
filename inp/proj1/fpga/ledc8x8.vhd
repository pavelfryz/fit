-- ledc8x8.vhd - Řízení maticového displeje
-- Author: Pavel Frýz <xfryzp00@stud.fit.vutbr.cz>

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- ------------------------ Entity declaration --------------------------------
entity ledc8x8 is
port (
  SMCLK: in std_logic;
  RESET: in std_logic;
  ROW: out std_logic_vector(0 to 7);
  LED: out std_logic_vector(0 to 7)
);
end entity ledc8x8;

-- ------------------------- Architecture declaration -------------------------
architecture ledc8x8_arch of ledc8x8 is

-- Propojovaci signaly
signal outled: std_logic_vector(0 to 7);
signal outrow: std_logic_vector(0 to 7);
signal ctrl_cnt: std_logic_vector(21 downto 0);
signal ce:std_logic;
signal switch:std_logic;

begin
-- Behavioralni popis 22-bitoveho synchroniho cyklickeho citace
ctrl_cnt_logic : process(RESET, SMCLK)
begin
  if (RESET = '1') then
    ctrl_cnt <= (others => '0');
  elsif (SMCLK'event AND SMCLK = '1') then
    ctrl_cnt <= ctrl_cnt + 1;
  end if;

  if (ctrl_cnt(7 downto 0) = "11111111") then
    ce <= '1';
  else
    ce <= '0';
  end if;

  switch <= ctrl_cnt(21);
end process ctrl_cnt_logic;

-- Behavioralni popis citace aktivace radku displeje
row_cnt_logic:process(ce,RESET,SMCLK)
begin
  if(RESET = '1') then
    outrow <= "01111111";
  elsif (SMCLK'event and SMCLK = '1') then
    if ce='1' then
      outrow <= outrow(7) & outrow(0 to 6);
    end if;
  end if;
end process row_cnt_logic;

-- Behavioralni popis dekoderu
dec_logic:process(outrow)
begin
  case outrow is
    when "01111111" => outled <= "11100000";
    when "10111111" => outled <= "10010000";
    when "11011111" => outled <= "10010000";
    when "11101111" => outled <= "11101111";
    when "11110111" => outled <= "10001000";
    when "11111011" => outled <= "10001110";
    when "11111101" => outled <= "00001000";
    when "11111110" => outled <= "00001000";
    when others =>     outled <= "00000000";
  end case;
end process dec_logic;

-- Behavioralni popis prvku pro realizaci efektu blikání(multiplexor)
switch_logic:process(switch,outled)
begin
  if switch='0' then
    LED <= "00000000";
  else
    LED <= outled;
  end if;
end process switch_logic;

ROW<=outrow;

end architecture ledc8x8_arch;
