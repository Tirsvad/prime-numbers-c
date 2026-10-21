# Cara ayak berfungsi

🌐 [English](How-Sieve-Works) · [Dansk](How-Sieve-Works-da) · **Bahasa Melayu**

`prime_sieve` (`src/prime_sieve.c`) tidak menguji nombor satu demi satu. Ia
menyenaraikan semua nombor dahulu dan memangkah yang tidak mungkin perdana. Yang
tinggal ialah nombor perdana. Ini ialah ayak Eratosthenes.

## Idea

1. Tulis nombor dari 2 hingga had.
2. Ambil nombor terkecil yang belum dipangkah. Ia perdana.
3. Pangkah gandaannya. Gandaan itu mempunyai faktor ini, jadi bukan perdana.
4. Ulang dari langkah 2 dengan nombor seterusnya yang belum dipangkah.

Dua fakta membolehkan program membuat kerja yang jauh lebih sedikit:

- **Mula pada p × p.** Gandaan `p` di bawah `p × p`, seperti `3p` atau `5p`,
  juga mempunyai faktor perdana yang lebih kecil, jadi perdana terdahulu sudah
  memangkahnya. Yang pertama yang baharu ialah `p × p`.
- **Berhenti pada sqrt(had).** Setiap nombor komposit sehingga had mempunyai
  faktor perdana paling banyak sama dengan punca kuasa duanya, jadi nombor
  perdana di atas sqrt(1,000,000) = 1,000 tiada lagi yang perlu dipangkah.

## Contoh kecil: nombor perdana hingga 50

Hanya nombor ganjil disimpan (lihat di bawah), jadi mulakan dengan 3, 5, 7, ...,
49:

| Perdana p | Bermula pada p × p | Dipangkah |
| --- | --- | --- |
| 3 | 9 | 9, 15, 21, 27, 33, 39, 45 |
| 5 | 25 | 25, 35, 45 |
| 7 | 49 | 49 |
| 9 | 81, melebihi 50 | berhenti: 9 × 9 melepasi had |

Gandaan ganjil `p` berjarak `2p` (`p × p`, `p × p + 2p`, ...), itulah
langkahnya. Yang tinggal ialah 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
47. Dengan 2 ditambah secara manual, itulah 15 nombor perdana hingga 50.

## Hanya nombor ganjil, satu bit setiap satu

Dua pilihan memastikan data kecil, supaya ia kekal dalam cache CPU:

- **Hanya nombor ganjil.** Setiap nombor genap melebihi 2 ialah komposit, jadi
  program melangkaunya dan menambah 2 secara manual. Bit `i` mewakili nombor
  ganjil `2i + 1`, jadi bit 0 ialah 1, bit 1 ialah 3, bit 2 ialah 5, dan
  seterusnya.
- **Satu bit setiap nombor.** Bit yang ditetapkan bermaksud "komposit". Bit
  dibungkus 64 dalam satu `uint64_t`. Sehingga 1,000,000 itu ialah 500,001 bit,
  kira-kira 7,800 perkataan atau 62 KB. Tatasusunan biasa satu bait setiap
  nombor akan menjadi 1 MB.

Untuk sampai ke bit `i`, perkataannya ialah `i >> 6` (iaitu `i / 64`) dan bit
di dalamnya ialah `i & 63` (iaitu `i % 64`).

## Kod, langkah demi langkah

```c
#define SIEVE_BITS (LIMIT / 2 + 1)
#define SIEVE_WORDS ((SIEVE_BITS + 63) / 64)
```

Bilangan bit, dan bilangan perkataan 64-bit yang memuatkannya (dibundarkan ke
atas).

```c
static uint64_t composite[SIEVE_WORDS];
memset(composite, 0, sizeof composite);
```

Setiap bit bermula pada 0: belum ada yang dipangkah.

```c
for (uint64_t p = 3; p * p <= LIMIT; p += 2) {
```

Cuba setiap `p` ganjil dari 3, dan berhenti apabila `p × p` melepasi had.
`p × p` dikira dalam 64 bit supaya tidak melimpah.

```c
    if (composite[(p >> 1) >> 6] >> ((p >> 1) & 63) & 1) continue;
```

Bit bagi `p` ialah `p >> 1` (kerana `p = 2i + 1`, maka `i = p >> 1`). Jika ia
ditetapkan, `p` telah dipangkah oleh perdana yang lebih kecil, jadi ia bukan
perdana dan dilangkau.

```c
    for (uint64_t m = (p * p) >> 1; m < SIEVE_BITS; m += p)
        composite[m >> 6] |= 1ull << (m & 63);
```

Pangkah gandaan ganjil `p`. Yang pertama ialah `p × p`, yang bitnya
`(p × p) >> 1`. Gandaan ganjil seterusnya lebih tinggi `2p` dalam nilai, iaitu
lebih tinggi `p` dalam indeks bit, jadi `m += p`. `|=` menetapkan bit dan tidak
mengganggu bit lain dalam perkataan itu.

```c
if (LIMIT >= 2) out[count++] = 2;
for (uint32_t i = 1; i < SIEVE_BITS; i++) {
    uint32_t n = 2 * i + 1;
    if (n > LIMIT) break;
    if (!(composite[i >> 6] >> (i & 63) & 1)) out[count++] = n;
}
```

Kumpulkan hasil: 2 dahulu, kemudian setiap `n = 2i + 1` ganjil yang bitnya
masih 0. Bit terakhir boleh jadi satu nombor melepasi had, jadi gelung berhenti
pada `n > LIMIT`.

## Mengapa lebih pantas

`prime_avx2` dan `prime_scalar` menguji setiap nombor secara berasingan: nombor
perdana hampir 1,000,000 memerlukan kira-kira 167 langkah gelung (pembahagi
6k ± 1 sehingga 1,000). Ayak tidak pernah membahagi. Setiap nombor komposit
dipangkah dengan beberapa penulisan bit yang mudah, kurang daripada sejuta
semuanya untuk 1,000,000 nombor, dan tatasusunan 62 KB muat dalam cache CPU.
Itulah sebabnya ia kira-kira 12 kali lebih pantas daripada versi AVX2 dan
kira-kira 23 kali lebih pantas daripada versi skalar (lihat
[Keputusan](Results-ms)).

Ayak hanya sesuai untuk seluruh julat. Untuk menyemak satu nombor,
`is_prime_avx2` ialah alat yang lebih baik, dan had yang lebih besar bermakna
tatasusunan yang lebih besar.
