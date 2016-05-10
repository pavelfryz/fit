/*
 * Soubor:  main.c
 * Datum:   2012/11/12
 * Autor:   Pavel Fryz, xfryzp00
 * Projekt: Klasicky budik s vystupem na VGA
 * Podil:   90%
 * Popis:   Pouziti dema vga_static, jako kostry pro vykreslovani,
 *          a keyb_lcd pro obsluhu klavesnice.
 *          Pridani funkci pro vykresleni jednotlivych casti
 *          hodin. Periodicke prekreslovani obrazovky pro zobrazeni
 *          aktualniho casu.
 */

#include "clock.h"

//aktualni cas
unsigned char t_hours = 0;  
unsigned char t_minutes = 0;
unsigned char t_seconds = 0;
//cas budiku
unsigned char a_hours = 0;
unsigned char a_minutes = 0;
unsigned char a_seconds = 0;
//posledne vykresleny cas
unsigned char d_hours = 0;
unsigned char d_minutes = 0;
unsigned char d_seconds = 0;
bool alarm = false;        //nastaveni budiku-vypnuto
bool draw = true;          //prekresleni hodin
bool draw_bell = true;     //prekresleni indikatoru budiku
bool draw_alarm = true;    //prekresleni casu budiku
bool draw_sun_moon = true; //prekresleni indikatoru stavu dne
bool ringing = false;      //zvoneni budiku-nezvoni

void print_user_help(void)
{
}

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return CMD_UNKNOWN;
}

void fpga_initialized(void)
{
}

int main(void)
{
  initialize_hardware();
  keyboard_init();
  LCD_init();
  LCD_clear();
  VGA_clear();
  VGA_draw_face();
  WDG_stop();
  set_led_d5(0);
  set_led_d6(0);

  CCTL0 = CCIE;
  CCR0 = 0x8000;
  TACTL = TASSEL_1 + MC_2;

  while (1) {
    keyboard_idle();
    terminal_idle();
    if (draw_bell) {
      VGA_draw_bell();
      draw_bell = false;
    }
    if (draw_alarm) {
      VGA_draw_alarm();
      draw_alarm = false;
    }
    if (draw_sun_moon) {
      VGA_draw_sun_moon();
      draw_sun_moon = false;
    }
    if (draw) {
      LCD_write_time();
      VGA_clear_hands();
      VGA_draw_hands();
      draw = false;
    }
  }
}

//Zpracovani stisku klavesnice
int keyboard_idle()
{
  char ch;
  static char last_ch = 0;
  ch = key_decode(read_word_keyboard_4x4());
  if (ch != last_ch) {
    last_ch = ch;
    switch (ch) {
    case 'A':
      t_hours++;
      t_hours %= 24;
      draw_sun_moon = true;
      draw = true;
      break;
    case 'B':
      t_minutes++;
      t_minutes %= 60;
      draw = true;
      break;
    case 'C':
      a_hours++;
      a_hours %= 24;
      draw_alarm = true;
      break;
    case 'D':
      a_minutes++;
      a_minutes %= 60;
      draw_alarm = true;
      break;
    case '*':
      t_seconds = 0;
      draw = true;
      break;
    case '#':
      alarm = !alarm;
      draw_bell = true;
      break;
    }
    if (ringing) {
      ringing = false;
      set_led_d6(0);
    }
  }
  return 0;
}

//Vykresli cifernik hodin
void VGA_draw_face()
{
  unsigned char i;
  color = VGA_BLACK;
  for (i = 0; i < FACE; i++) {
    DRAW_REL_LINE(face_x[i], face_y[i], seconds_x[i], seconds_y[i]);
    DRAW_REL_LINE(-face_x[i], face_y[i], -seconds_x[i], seconds_y[i]);
    DRAW_REL_LINE(face_x[i], -face_y[i], seconds_x[i], -seconds_y[i]);
    DRAW_REL_LINE(-face_x[i], -face_y[i], -seconds_x[i], -seconds_y[i]);
    DRAW_REL_LINE(face_y[i], face_x[i], seconds_y[i], seconds_x[i]);
    DRAW_REL_LINE(-face_y[i], face_x[i], -seconds_y[i], seconds_x[i]);
    DRAW_REL_LINE(face_y[i], -face_x[i], seconds_y[i], -seconds_x[i]);
    DRAW_REL_LINE(-face_y[i], -face_x[i], -seconds_y[i], -seconds_x[i]);
  }
}

//Prekresli rucicky barvou pozadi
//a opravi cifernik
void VGA_clear_hands()
{
  color = VGA_BACKGROUND;
  draw_hand(d_seconds, center, center, seconds_x, seconds_y);
  draw_hand(d_minutes, center, center, minutes_x, minutes_y);
  draw_hand(5 * (d_hours % 12) + d_minutes / 12, center, center, hours_x, hours_y);
  color = VGA_BLACK;
  draw_hand(d_seconds, face_x, face_y, seconds_x, seconds_y);
  draw_hand(d_minutes, face_x, face_y, seconds_x, seconds_y);
}

//Vykresli rucicky
void VGA_draw_hands()
{
  d_seconds = t_seconds;
  d_minutes = t_minutes;
  d_hours = t_hours;
  color = VGA_GRAY;
  draw_hand(d_seconds, center, center, seconds_x, seconds_y);
  color = VGA_BLUE;
  draw_hand(d_minutes, center, center, minutes_x, minutes_y);
  color = VGA_LIGHT_BLUE;
  draw_hand(5 * (d_hours % 12) + d_minutes / 12, center, center, hours_x, hours_y);
}

//Pomocna funkce pro vykresleni rucicek
//pos - posize rucicky(0-59)
void draw_hand(unsigned char pos,
               const unsigned char *x1,
               const unsigned char *y1,
               const unsigned char *x2, const unsigned char *y2)
{
  if (pos < 8) {
    DRAW_REL_LINE(x1[pos], -y1[pos], x2[pos], -y2[pos]);
  }
  else if (pos < 16) {
    DRAW_REL_LINE(y1[15 - pos], -x1[15 - pos], y2[15 - pos], -x2[15 - pos]);
  }
  else if (pos < 23) {
    DRAW_REL_LINE(y1[pos - 15], x1[pos - 15], y2[pos - 15], x2[pos - 15]);
  }
  else if (pos < 30) {
    DRAW_REL_LINE(x1[30 - pos], y1[30 - pos], x2[30 - pos], y2[30 - pos]);
  }
  else if (pos < 38) {
    DRAW_REL_LINE(-x1[pos - 30], y1[pos - 30], -x2[pos - 30], y2[pos - 30]);
  }
  else if (pos < 46) {
    DRAW_REL_LINE(-y1[45 - pos], x1[45 - pos], -y2[45 - pos], x2[45 - pos]);
  }
  else if (pos < 53) {
    DRAW_REL_LINE(-y1[pos - 45], -x1[pos - 45], -y2[pos - 45], -x2[pos - 45]);
  }
  else {
    DRAW_REL_LINE(-x1[60 - pos], -y1[60 - pos], -x2[60 - pos], -y2[60 - pos]);
  }
}

//Vyplni obrazovku barvou pozadi
void VGA_clear(void)
{
  unsigned short x, y;
  color = VGA_BACKGROUND;
  for (y = 0; y < VGA_ROWS; y++)
    for (x = 0; x < VGA_COLUMNS; x++) {
      PUT_PIXEL(x, y);
    }
}

//Vykresli cas budiku
void VGA_draw_alarm()
{
  unsigned char x, y;
  color = VGA_BLACK;
  PUT_PIXEL(COLON_X, COLON1_Y);
  PUT_PIXEL(COLON_X, COLON2_Y);
  unsigned char h1 = a_hours / 10, h2 = a_hours % 10, m1 = a_minutes / 10, m2 = a_minutes % 10;
  for (y = 0; y < NUMS_HEIGHT; y++)
    for (x = 0; x < NUMS_WIDTH; x++) {
      PUT_PIXEL_C(NUM1_X + x, NUM_Y + y, NUMS(x, y, h1));
      PUT_PIXEL_C(NUM2_X + x, NUM_Y + y, NUMS(x, y, h2));
      PUT_PIXEL_C(NUM3_X + x, NUM_Y + y, NUMS(x, y, m1));
      PUT_PIXEL_C(NUM4_X + x, NUM_Y + y, NUMS(x, y, m2));
    }
}

//Vykresli indikator dne(den-noc)
void VGA_draw_sun_moon()
{
  unsigned short x, y;
  if (t_hours < 7 || t_hours >= 19) {
    for (y = 0; y < SUN_MOON_HEIGHT; y++)
      for (x = 0; x < SUN_MOON_WIDTH; x++) {
        color = MOON(x, y);
        PUT_PIXEL(SUN_MOON_X + x, SUN_MOON_Y + y);
      }
  }
  else {
    for (y = 0; y < SUN_MOON_HEIGHT; y++)
      for (x = 0; x < SUN_MOON_WIDTH; x++) {
        color = SUN(x, y);
        PUT_PIXEL(SUN_MOON_X + x, SUN_MOON_Y + y);
      }
  }
}

//Vykresli indikator stavu budiku(zap-vyp)
void VGA_draw_bell()
{
  if (alarm) {
    color = VGA_BACKGROUND;
    draw_line(BELL_X - 1, BELL_Y - 1, BELL_X + BELL_WIDTH, BELL_Y + BELL_HEIGHT);
    draw_line(BELL_X - 1, BELL_Y + BELL_HEIGHT, BELL_X + BELL_WIDTH, BELL_Y - 1);
  }

  unsigned short x, y;
  for (y = 0; y < BELL_HEIGHT; y++)
    for (x = 0; x < BELL_WIDTH; x++) {
      color = BELL(x, y);
      if (color != VGA_BACKGROUND)
        PUT_PIXEL(BELL_X + x, BELL_Y + y);
    }

  if (!alarm) {
    color = VGA_RED;
    draw_line(BELL_X - 1, BELL_Y - 1, BELL_X + BELL_WIDTH, BELL_Y + BELL_HEIGHT);
    draw_line(BELL_X - 1, BELL_Y + BELL_HEIGHT, BELL_X + BELL_WIDTH, BELL_Y - 1);
  }
}

//Napise aktualni cas na lcd display
void LCD_write_time()
{
  char buffer[10];
  sprintf(buffer, "%02d:%02d:%02d", t_hours, t_minutes, t_seconds);
  LCD_write_string(buffer);
}

//Funkce na vykresleni usecky
//x1,y1 pocatecni souradnice
//x2,y2 koncova souradnice
void draw_line(char x1, char y1, char x2, char y2)
{
  bool steep = false;
  char absy = (y2 > y1) ? y2 - y1 : y1 - y2;
  char absx = (x2 > x1) ? x2 - x1 : x1 - x2;
  if (absy > absx) {
    steep = true;
    SWAP(x1, y1);
    SWAP(x2, y2);
  }
  if (x1 > x2) {
    SWAP(x1, x2);
    SWAP(y1, y2);
  }

  char dx = x2 - x1, dy = (y2 > y1) ? y2 - y1 : y1 - y2;
  char P1 = 2 * dy, P2 = P1 - 2 * dx;
  char P = 2 * dy - dx;
  char y = y1;
  char x;
  for (x = x1; x <= x2; x++) {
    if (steep) {
      PUT_PIXEL(y, x);
    }
    else {
      PUT_PIXEL(x, y);
    }
    if (P >= 0) {
      P += P2;
      if (y1 > y2) {
        y--;
      }
      else {
        y++;
      }
    }
    else {
      P += P1;
    }
  }
}

interrupt(TIMERA0_VECTOR) Timer_A(void)
{
  flip_led_d5();
  CCR0 += 0x8000;

  t_seconds++;
  if (t_seconds == 60) {
    t_minutes++;
    t_seconds = 0;
  }
  if (t_minutes == 60) {
    t_hours++;
    draw_sun_moon = true;
    t_minutes = 0;
  }
  if (t_hours == 24) {
    t_hours = 0;
  }
  if (alarm && t_hours == a_hours && t_minutes == a_minutes && t_seconds == 0) {
    ringing = true;
  }
  if (ringing) {
    flip_led_d6();
  }
  draw = true;
}
