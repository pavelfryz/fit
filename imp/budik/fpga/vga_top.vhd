--
-- Soubor:  vga_top.vhd
-- Datum:   2012/11/12
-- Autor:   Pavel Fryz, xfryzp00
-- Projekt: Klasicky budik s vystupem na VGA
-- Podil:   0%
-- Popis:   Spojeno z demo pro vga_static a keyb_lcd
--

library IEEE;
use IEEE.std_logic_1164.ALL;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.vga_controller_cfg.all;
use work.clkgen_cfg.all;

architecture arch_vga_mode of tlv_pc_ifc is

   signal irgb : std_logic_vector(8 downto 0);
   signal rrow : std_logic_vector(11 downto 0);
   signal rcol : std_logic_vector(11 downto 0);
   signal border: std_ulogic;

   -- Port A VIDEORAM
   signal spi_vram_addr    : std_logic_vector (12 downto 0); 
   signal spi_vram_data_out: std_logic_vector (7 downto 0); 
   signal spi_vram_data_in : std_logic_vector (7 downto 0); 
   signal spi_vram_data_in0 : std_logic_vector (7 downto 0); 
   signal spi_vram_data_in1 : std_logic_vector (7 downto 0); 
   signal spi_vram_write_en: std_logic; 
   signal spi_vram_read_en : std_logic;
   signal vram0_en : std_logic;
   signal vram1_en : std_logic;

   -- Port B VGARAM
   signal vga_bram_addr    : std_logic_vector (12 downto 0); 
   signal vga_bram_dout    : std_logic_vector (3 downto 0); 
  
   signal vga_mode  : std_logic_vector(60 downto 0); -- default 640x480x60
   
   -- Keyboard 4x4
   signal key_data_in : std_logic_vector (15 downto 0);

   -- displej
   signal dis_addr     : std_logic_vector(0 downto 0);
   signal dis_data_out : std_logic_vector(15 downto 0);
   signal dis_write_en : std_logic;
   signal led          : std_logic;
   
   -- SPI interface
   component SPI_adc
      generic (
         ADDR_WIDTH : integer;
         DATA_WIDTH : integer;
         ADDR_OUT_WIDTH : integer;
         BASE_ADDR  : integer;
         DELAY : integer := 0
      ); 
      port ( 
         CLK      : in  std_logic;  
  
         CS       : in  std_logic;
         DO       : in  std_logic;
         DO_VLD   : in  std_logic;
         DI       : out std_logic;
         DI_REQ   : in  std_logic;
   
         ADDR     : out  std_logic_vector (ADDR_OUT_WIDTH-1 downto 0);
         DATA_OUT : out  std_logic_vector (DATA_WIDTH-1 downto 0);
         DATA_IN  : in   std_logic_vector (DATA_WIDTH-1 downto 0);
   
         WRITE_EN : out  std_logic;
         READ_EN  : out  std_logic 
      );    
   end component; 
   
   -- Keyboard 4x4
   component keyboard_controller
      port(
         CLK      : in std_logic;
         RST      : in std_logic;

         DATA_OUT : out std_logic_vector(15 downto 0);
         DATA_VLD : out std_logic;

         KB_KIN   : out std_logic_vector(3 downto 0);
         KB_KOUT  : in  std_logic_vector(3 downto 0)
      );
   end component;
   
   component lcd_raw_controller
      port (
         RST      : in std_logic;
         CLK      : in std_logic;

         DATA_IN  : in std_logic_vector (15 downto 0);
         WRITE_EN : in std_logic;

         DISPLAY_RS   : out   std_logic;
         DISPLAY_DATA : inout std_logic_vector(7 downto 0);
         DISPLAY_RW   : out   std_logic;
         DISPLAY_EN   : out   std_logic
      );
   end component;

   type ramtt is array (2**13 - 1 downto 0) of std_logic_vector (3 downto 0);
   signal ram: ramtt;

begin
   -- Set graphical mode (640x480, 60 Hz refresh)
   setmode(r640x480x60, vga_mode);
    
   -- spi decoder (videoRAM)
   SPI_adc_ram: SPI_adc
      generic map(
         ADDR_WIDTH => 16,
         DATA_WIDTH => 8,
         ADDR_OUT_WIDTH => 13,
         BASE_ADDR  => 16#8000#, -- memory address range is 0x8000 - 0x9FFF 
         DELAY => 1
      )
      port map(
         CLK      => CLK,
         CS       => SPI_CS,    

         DO       => SPI_DO,
         DO_VLD   => SPI_DO_VLD,
         DI       => SPI_DI,
         DI_REQ   => SPI_DI_REQ, 

         ADDR     => spi_vram_addr,
         DATA_OUT => spi_vram_data_out,
         DATA_IN  => spi_vram_data_in,
         WRITE_EN => spi_vram_write_en,
         READ_EN  => open
      );

      process (CLK)
      begin
        if (CLK'event and CLK = '1') then
           if (spi_vram_write_en = '1') then
              ram(conv_integer(spi_vram_addr)) <= spi_vram_data_out(3 downto 0);
           end if;
           spi_vram_data_in <= "0000" & ram(conv_integer(spi_vram_addr));

           vga_bram_dout <= ram(conv_integer(vga_bram_addr));
        end if;
      end process;

   -- VGA controller, delay 1 tact
   vga: entity work.vga_controller(arch_vga_controller) 
      generic map (REQ_DELAY => 1)
      port map (
         CLK    => CLK, 
         RST    => RESET,
         ENABLE => '1',
         MODE   => vga_mode,

         DATA_RED    => irgb(8 downto 6),
         DATA_GREEN  => irgb(5 downto 3),
         DATA_BLUE   => irgb(2 downto 0),
         ADDR_COLUMN => rcol,
         ADDR_ROW    => rrow,

         VGA_RED   => RED_V,
         VGA_BLUE  => BLUE_V,
         VGA_GREEN => GREEN_V,
         VGA_HSYNC => HSYNC_V,
         VGA_VSYNC => VSYNC_V
      );

   -- Convert request for VGA (row, column) into VideoRAM address
   vga_bram_addr <= rrow(8 downto 3) & rcol(9 downto 3);
  
   -- Choose color based on value returned from memory
   select_color: process(vga_bram_dout)
   begin
        -- Decode color from palette
        case vga_bram_dout is
            when "0001" => irgb <= "111111111"; -- white
            when "0010" => irgb <= "100100100"; -- gray
            when "0011" => irgb <= "111111000"; -- yellow
            when "0100" => irgb <= "000000111"; -- blue
            when "0101" => irgb <= "110100111"; -- light magenta
            when "0110" => irgb <= "000100010"; -- dark green
            when "0111" => irgb <= "100111000"; -- light green
            when "1000" => irgb <= "100111111"; -- light cyan
            when "1001" => irgb <= "000111000"; -- green
            when "1010" => irgb <= "000100111"; -- light blue
            when "1011" => irgb <= "111001000"; -- light red
            when "1100" => irgb <= "110100011"; -- light brown
            when "1101" => irgb <= "111000000"; -- red
            when "1110" => irgb <= "100001110"; -- magenta
            when "1111" => irgb <= "000011101"; -- cyan
            -- Otherwise black color
            when others =>
               irgb <= "000000000";
        end case;
   end process;
   
   LEDF  <= '0';

   -- Adresovy dekoder
   SPI_adc_keybrd: SPI_adc
      generic map(
         ADDR_WIDTH => 8,       -- sirka adresy 8 bitu
         DATA_WIDTH => 16,      -- sirka dat 16 bitu
         ADDR_OUT_WIDTH => 1,   -- sirka adresy na vystupu min. 1 bit
         BASE_ADDR  => 16#0002# -- adresovy prostor od 0x0002
      )
      port map(
         CLK   =>  CLK,
   
         CS       => SPI_CS,
         DO       => SPI_DO,
         DO_VLD   => SPI_DO_VLD,
         DI       => SPI_DI,
         DI_REQ   => SPI_DI_REQ,
   
         ADDR     => open,
         DATA_OUT => open,
         DATA_IN  => key_data_in,
         WRITE_EN => open,
         READ_EN  => open
      );
     
   -- Radic klavesnice
   keybrd: keyboard_controller
      port map(
         CLK      => CLK,
         RST      => RESET,       -- reset
         
         DATA_OUT => key_data_in,         
         DATA_VLD => open,

         -- Keyboart
         KB_KIN   => KIN,
         KB_KOUT  => KOUT
      );
      
   -- SPI dekoder pro displej
   spidecd: SPI_adc
         generic map (
            ADDR_WIDTH => 8,     -- sirka adresy 8 bitu
            DATA_WIDTH => 16,    -- sirka dat 16 bitu
            ADDR_OUT_WIDTH => 1, -- sirka adresy na vystupu 1 bit
            BASE_ADDR  => 16#00# -- adresovy prostor od 0x00-0x01
         )
         port map (
            CLK      => CLK,
            CS       => SPI_CS,

            DO       => SPI_DO,
            DO_VLD   => SPI_DO_VLD,
            DI       => SPI_DI,
            DI_REQ   => SPI_DI_REQ,

            ADDR     => dis_addr,
            DATA_OUT => dis_data_out,
            DATA_IN  => "0000000000000000",
            WRITE_EN => dis_write_en,
            READ_EN  => open
         );

  
   -- radic LCD displeje
   lcdctrl: lcd_raw_controller
         port map (
            RST    =>  RESET,
            CLK    =>  CLK,

            -- ridici signaly
            DATA_IN  => dis_data_out,
            WRITE_EN => dis_write_en,

            --- signaly displeje
            DISPLAY_RS   => LRS,
            DISPLAY_DATA => LD,
            DISPLAY_RW   => LRW,
            DISPLAY_EN   => LE
         );
end arch_vga_mode;
