#include "raylib.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SCREEN_W 160
#define SCREEN_H 144

// =============================================================================
// HELPERS
// =============================================================================

int read_file(char **data_buffer, char const *file_name)
{
  FILE *f_ptr = fopen(file_name, "r");
  if (!f_ptr)
  {
    printf("failed to open file: %s\n", file_name);
    return -1;
  }
  if (fseek(f_ptr, 0, SEEK_END))
  {
    fclose(f_ptr);
    printf("fseek failed\n");
    return -1;
  }
  long size = ftell(f_ptr);
  if (size < 0)
  {
    fclose(f_ptr);
    printf("ftell failed\n");
    return -1;
  }
  rewind(f_ptr);
  *data_buffer = malloc(size + 1);
  if (fread(*data_buffer, 1, size, f_ptr) < size)
  {
    fclose(f_ptr);
    printf("failed to read file: %s\n", file_name);
    free(*data_buffer);
    return -1;
  }
  fclose(f_ptr);
  (*data_buffer)[size] = '\0';
  return 0;
}


void usage()
{
  printf("Usage: gameboy <rom-fielpath>\n");
}

// =============================================================================
// MEMORY
// =============================================================================

#define BANK_SIZE 16384

uint8_t  ram[8192];     // C000 -> DFFF
uint8_t  vram[8192];    // 8000 -> 9FFF
uint8_t  oam[160];      // FE00 -> FE9F
uint8_t  io_regs[128];  // FF00 -> FF7F
uint8_t  hram[127];     // FF80 -> FFFE
uint8_t  ie;            // FFFF
uint32_t cart_ram_size;
uint8_t *cart_ram;
uint32_t rom_ram_size;
uint8_t *rom_ram;

typedef struct {
  uint16_t rom_bank;
  uint8_t  ram_bank;
  bool     ram_enabled;
  uint8_t  mbc1_bank_lo;
  uint8_t  mbc1_bank_hi;
  bool     mbc1_mode;
} MBC;

// =============================================================================
// CPU
// =============================================================================

// labeled: "a", "b", "c", "d", "e", "f", "h", "l"
uint8_t registers[8];

uint16_t read_16bit_vreg(uint8_t idx1, uint8_t idx2)
{
  uint16_t reg1 = registers[idx1];
  uint16_t reg2 = registers[idx2];
  return (reg1 << 8) | reg2;
}

void write_16bit_vreg(uint8_t idx1, uint8_t idx2, uint16_t value)
{
  registers[idx1] = (uint8_t)((value & 0xFF00) >> 8);
  registers[idx2] = (uint8_t)(value & 0xFF);
}

uint16_t read_af()
{
  return read_16bit_vreg(0, 5);
}

void write_af(uint16_t value)
{
  write_16bit_vreg(0, 5, value);
}

uint16_t read_bc()
{
  return read_16bit_vreg(1, 2);
}

void write_bc(uint16_t value)
{
  return write_16bit_vreg(1, 2, value);
}

uint16_t read_de()
{
  return read_16bit_vreg(3, 4);
}

void write_de(uint16_t value)
{
  return write_16bit_vreg(3, 4, value);
}

uint16_t read_hl()
{
  return read_16bit_vreg(6, 7);
}

void write_hl(uint16_t value)
{
  return write_16bit_vreg(6, 7, value);
}

// TODO: read/write to bits in the flag register, f

// =============================================================================
// MAIN
// =============================================================================

int main(int const argc, char const *argv[])
{
  if (argc != 2 || !strcmp(argv[1], "--help"))
  {
    usage();
    return 0;
  }
  char *rom_data;
  read_file(&rom_data, argv[1]);
  rom_ram = (uint8_t *)rom_data;

  InitWindow(SCREEN_W, SCREEN_H, "GameBoy");

  // TODO: framerate: ca. 59.7275hz
  SetTargetFPS(60);  // this is probably too simple once i consider linerates
                     // and do the rendering

  while (!WindowShouldClose())
  {
    ClearBackground(WHITE);
    BeginDrawing();

    static float x = 0, y = 0;
    DrawRectangle(x += GetFrameTime(), y += GetFrameTime(), 20, 20, RED);

    EndDrawing();
  }

  CloseWindow();
  printf("Shut down normally\n");
}
