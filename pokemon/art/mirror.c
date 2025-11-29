#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE 8192  /* máximo de bytes por línea */

/* --- Decodificación UTF-8 a code points --- */

size_t utf8_decode_line(const char *s, size_t len, uint32_t *out, size_t out_cap) {
    size_t i = 0;
    size_t n = 0;

    while (i < len && n < out_cap) {
        unsigned char c = (unsigned char)s[i];
        uint32_t cp;
        size_t extra = 0;

        if (c < 0x80) {
            cp = c;
            extra = 0;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < len) {
            cp = (c & 0x1F) << 6;
            cp |= (unsigned char)s[i + 1] & 0x3F;
            extra = 1;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < len) {
            cp = (c & 0x0F) << 12;
            cp |= ((unsigned char)s[i + 1] & 0x3F) << 6;
            cp |= (unsigned char)s[i + 2] & 0x3F;
            extra = 2;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < len) {
            cp = (c & 0x07) << 18;
            cp |= ((unsigned char)s[i + 1] & 0x3F) << 12;
            cp |= ((unsigned char)s[i + 2] & 0x3F) << 6;
            cp |= (unsigned char)s[i + 3] & 0x3F;
            extra = 3;
        } else {
            /* byte raro: lo tratamos como un carácter suelto */
            cp = c;
            extra = 0;
        }

        out[n++] = cp;
        i += 1 + extra;
    }

    return n;
}

/* --- Encodificación de code points a UTF-8 --- */

size_t utf8_encode_line(const uint32_t *codepoints, size_t n,
                        char *out, size_t out_cap) {
    size_t pos = 0;

    for (size_t i = 0; i < n; i++) {
        uint32_t cp = codepoints[i];

        if (cp <= 0x7F) {
            if (pos + 1 >= out_cap) break;
            out[pos++] = (char)cp;
        } else if (cp <= 0x7FF) {
            if (pos + 2 >= out_cap) break;
            out[pos++] = (char)(0xC0 | (cp >> 6));
            out[pos++] = (char)(0x80 | (cp & 0x3F));
        } else if (cp <= 0xFFFF) {
            if (pos + 3 >= out_cap) break;
            out[pos++] = (char)(0xE0 | (cp >> 12));
            out[pos++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            out[pos++] = (char)(0x80 | (cp & 0x3F));
        } else if (cp <= 0x10FFFF) {
            if (pos + 4 >= out_cap) break;
            out[pos++] = (char)(0xF0 | (cp >> 18));
            out[pos++] = (char)(0x80 | ((cp >> 12) & 0x3F));
            out[pos++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            out[pos++] = (char)(0x80 | (cp & 0x3F));
        } else {
            /* fuera de rango → � (U+FFFD) */
            if (pos + 3 >= out_cap) break;
            uint32_t r = 0xFFFD;
            out[pos++] = (char)(0xE0 | (r >> 12));
            out[pos++] = (char)(0x80 | ((r >> 6) & 0x3F));
            out[pos++] = (char)(0x80 | (r & 0x3F));
        }
    }

    if (pos < out_cap) {
        out[pos] = '\0';
    }

    return pos;
}

/* --- Mirror de una celda braille (U+2800–U+28FF) --- */

uint32_t mirror_braille_cell(uint32_t cp) {
    if (cp < 0x2800 || cp > 0x28FF) {
        return cp; /* no es braille, lo dejamos igual */
    }

    uint32_t dots = cp - 0x2800;  /* bits 0..7 = puntos 1..8 */
    uint32_t nd = 0;

    /* columna izquierda: 1,2,3,7  ↔  derecha: 4,5,6,8 */

    /* 1 -> 4, 4 -> 1 */
    if (dots & (1u << 0)) nd |= (1u << 3);
    if (dots & (1u << 3)) nd |= (1u << 0);

    /* 2 -> 5, 5 -> 2 */
    if (dots & (1u << 1)) nd |= (1u << 4);
    if (dots & (1u << 4)) nd |= (1u << 1);

    /* 3 -> 6, 6 -> 3 */
    if (dots & (1u << 2)) nd |= (1u << 5);
    if (dots & (1u << 5)) nd |= (1u << 2);

    /* 7 -> 8, 8 -> 7 */
    if (dots & (1u << 6)) nd |= (1u << 7);
    if (dots & (1u << 7)) nd |= (1u << 6);

    return 0x2800 + nd;
}

/* --- Mirror de una línea UTF-8, con braille --- */

void mirror_line_utf8(const char *line_in, char *line_out, size_t out_cap) {
    size_t len = strlen(line_in);
    int has_nl = (len > 0 && line_in[len - 1] == '\n');
    size_t text_len;
    if (has_nl)
        text_len = (len - 1);
    else
        text_len = len;

    uint32_t cps[MAX_LINE];  /* code points */
    size_t n_cps = utf8_decode_line(line_in, text_len, cps, MAX_LINE);

    /* Invertir orden de los code points */
    for (size_t i = 0; i < n_cps / 2; i++) {
        uint32_t tmp = cps[i];
        cps[i] = cps[n_cps - 1 - i];
        cps[n_cps - 1 - i] = tmp;
    }

    /* Aplicar mirror por celda en los braille */
    for (size_t i = 0; i < n_cps; i++) {
        cps[i] = mirror_braille_cell(cps[i]);
    }

    /* Re-encodificar */
    size_t pos = utf8_encode_line(cps, n_cps, line_out, out_cap);

    if (has_nl && pos + 1 < out_cap) {
        line_out[pos++] = '\n';
        line_out[pos] = '\0';
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr,
                "Uso: %s archivo_entrada\n"
                "Lee un archivo UTF-8 (incluyendo braille) y escribe el mirror horizontal en stdout.\n"
                "Ejemplo: %s arte.txt > arte_mirror.txt\n",
                argv[0], argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    char buffer[MAX_LINE];
    char outbuf[MAX_LINE * 4];

    while (fgets(buffer, sizeof buffer, f) != NULL) {
        mirror_line_utf8(buffer, outbuf, sizeof outbuf);
        fputs(outbuf, stdout);
    }

    if (ferror(f)) {
        perror("Error leyendo el archivo");
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}
