
#ifndef LCD_H
#define LCD_H

  void LCD_reset (void);
  void LCD_init (void);
  void writeLCD (char * str, int line);
  void flush_buffer (void);

#endif
