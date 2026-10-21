// The three prime programs of the C version in one Rust program:
//   prime scalar   plain trial division by 6k +/- 1
//   prime avx2     AVX2 trial division by 6k +/- 1, four divisors per step
//   prime sieve    sieve of Eratosthenes over odd numbers, one bit per number
// Primes go to standard output, the timing line to standard error.
use std::arch::x86_64::*;
use std::io::{BufWriter, Write};
use std::time::Instant;

const LIMIT: u32 = 1_000_000;

// Returns true if n is prime, using plain scalar trial division by 6k +/- 1.
fn is_prime_scalar(n: u32) -> bool {
    if n <= 1 {
        return false;
    }
    if n <= 3 {
        return true;
    }
    if n % 2 == 0 || n % 3 == 0 {
        return false;
    }
    // Every prime above 3 is 6k - 1 or 6k + 1: test d and d + 2, then move 6
    // forward. Stop once d * d > n. The divide is 64-bit here; the compiler
    // narrows it to a 32-bit divide because n and d fit in 32 bits.
    let n64 = n as u64;
    let mut d: u64 = 5;
    while d * d <= n64 {
        if n64 % d == 0 || n64 % (d + 2) == 0 {
            return false;
        }
        d += 6;
    }
    true
}

// Returns true if n is prime, using AVX2 trial division by 6k +/- 1.
// Four divisors are tested per iteration in double precision, which holds every
// 32-bit integer exactly, so floor(n / d) is exact.
#[target_feature(enable = "avx2")]
unsafe fn is_prime_avx2(n: u64) -> bool {
    if n <= 1 {
        return false;
    }
    if n <= 3 {
        return true;
    }
    if n % 2 == 0 || n % 3 == 0 {
        return false;
    }
    if n > u32::MAX as u64 {
        return false; // out of supported range
    }

    // Only divisors up to sqrt(n) need testing.
    let limit = (n as f64).sqrt().floor();
    let v_n = _mm256_set1_pd(n as f64);
    let v_limit = _mm256_set1_pd(limit);

    // Divisors 5, 7, 11, 13; each iteration advances by 12.
    let mut v_div_i = _mm_setr_epi32(5, 7, 11, 13);
    let v_step = _mm_set1_epi32(12);

    let mut base = 5.0f64;
    while base <= limit {
        let v_d = _mm256_cvtepi32_pd(v_div_i);
        // rem = n - floor(n / d) * d, which is n mod d, for 4 divisors at once.
        let v_q = _mm256_floor_pd(_mm256_div_pd(v_n, v_d));
        let v_rem = _mm256_sub_pd(v_n, _mm256_mul_pd(v_q, v_d));

        // A lane counts if it divides n and the divisor is <= sqrt(n).
        let is_zero = _mm256_cmp_pd::<_CMP_EQ_OQ>(v_rem, _mm256_setzero_pd());
        let in_range = _mm256_cmp_pd::<_CMP_LE_OQ>(v_d, v_limit);
        if _mm256_movemask_pd(_mm256_and_pd(is_zero, in_range)) != 0 {
            return false;
        }

        v_div_i = _mm_add_epi32(v_div_i, v_step);
        base += 12.0;
    }
    true
}

// Sieve of Eratosthenes over odd numbers. Bit i of the array stands for the odd
// number 2i + 1, and a set bit means "composite".
fn sieve_primes() -> Vec<u32> {
    let sieve_bits = (LIMIT / 2 + 1) as usize;
    let mut composite = vec![0u64; (sieve_bits + 63) / 64];

    // Cross out odd multiples of each odd prime p, starting at p * p.
    let mut p: u64 = 3;
    while p * p <= LIMIT as u64 {
        let pi = (p >> 1) as usize;
        if (composite[pi >> 6] >> (pi & 63)) & 1 == 0 {
            // Consecutive odd multiples of p are p apart in bit index.
            let mut m = ((p * p) >> 1) as usize;
            while m < sieve_bits {
                composite[m >> 6] |= 1u64 << (m & 63);
                m += p as usize;
            }
        }
        p += 2;
    }

    // 2 is the only even prime; then every odd number whose bit is still 0.
    let mut primes = vec![2];
    for i in 1..sieve_bits {
        let n = 2 * i as u32 + 1;
        if n > LIMIT {
            break;
        }
        if (composite[i >> 6] >> (i & 63)) & 1 == 0 {
            primes.push(n);
        }
    }
    primes
}

fn main() {
    let which = std::env::args().nth(1).unwrap_or_default();
    let mut primes: Vec<u32> = Vec::with_capacity(LIMIT as usize / 2);

    // Time the computation only; printing is done afterwards.
    let t0 = Instant::now();
    match which.as_str() {
        "scalar" => {
            for n in 0..=LIMIT {
                if is_prime_scalar(n) {
                    primes.push(n);
                }
            }
        }
        "avx2" => {
            for n in 0..=LIMIT {
                if unsafe { is_prime_avx2(n as u64) } {
                    primes.push(n);
                }
            }
        }
        "sieve" => primes = sieve_primes(),
        _ => {
            eprintln!("usage: prime scalar|avx2|sieve");
            std::process::exit(2);
        }
    }
    let ms = t0.elapsed().as_secs_f64() * 1000.0;

    let stdout = std::io::stdout();
    let mut out = BufWriter::new(stdout.lock());
    for p in &primes {
        writeln!(out, "{}", p).unwrap();
    }
    out.flush().unwrap();

    eprintln!("Found {} primes from 0 to {} in {:.3} ms", primes.len(), LIMIT, ms);
}
