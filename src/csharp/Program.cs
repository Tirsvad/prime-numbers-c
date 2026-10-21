// The three prime programs of the C version in one C# program:
//   dotnet prime.dll scalar   plain trial division by 6k +/- 1
//   dotnet prime.dll avx2     AVX2 trial division by 6k +/- 1, four divisors per step
//   dotnet prime.dll sieve    sieve of Eratosthenes over odd numbers, one bit per number
// Primes go to standard output, the timing line to standard error.
using System.Diagnostics;
using System.Runtime.Intrinsics;
using System.Runtime.Intrinsics.X86;
using System.Text;

const uint Limit = 1_000_000;

// Returns true if n is prime, using plain scalar trial division by 6k +/- 1.
static bool IsPrimeScalar(uint n)
{
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    // Every prime above 3 is 6k - 1 or 6k + 1: test d and d + 2, then move 6
    // forward. Stop once d * d > n.
    for (ulong d = 5; d * d <= n; d += 6)
        if (n % d == 0 || n % (d + 2) == 0)
            return false;
    return true;
}

// Returns true if n is prime, using AVX2 trial division by 6k +/- 1.
// Four divisors are tested per iteration in double precision, which holds every
// 32-bit integer exactly, so floor(n / d) is exact.
static bool IsPrimeAvx2(ulong n)
{
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    if (n > uint.MaxValue) return false; // out of supported range

    // Only divisors up to sqrt(n) need testing.
    double limit = Math.Floor(Math.Sqrt((double)n));
    Vector256<double> vN = Vector256.Create((double)n);
    Vector256<double> vLimit = Vector256.Create(limit);

    // Divisors 5, 7, 11, 13; each iteration advances by 12.
    Vector128<int> vDivI = Vector128.Create(5, 7, 11, 13);
    Vector128<int> vStep = Vector128.Create(12);

    for (double b = 5.0; b <= limit; b += 12.0)
    {
        Vector256<double> vD = Avx.ConvertToVector256Double(vDivI);
        // rem = n - floor(n / d) * d, which is n mod d, for 4 divisors at once.
        Vector256<double> vQ = Avx.Floor(Avx.Divide(vN, vD));
        Vector256<double> vRem = Avx.Subtract(vN, Avx.Multiply(vQ, vD));

        // A lane counts if it divides n and the divisor is <= sqrt(n).
        Vector256<double> isZero = Avx.Compare(vRem, Vector256<double>.Zero, FloatComparisonMode.OrderedEqualNonSignaling);
        Vector256<double> inRange = Avx.Compare(vD, vLimit, FloatComparisonMode.OrderedLessThanOrEqualNonSignaling);
        if (Avx.MoveMask(Avx.And(isZero, inRange)) != 0)
            return false;

        vDivI = Sse2.Add(vDivI, vStep);
    }
    return true;
}

// Sieve of Eratosthenes over odd numbers. Bit i of the array stands for the odd
// number 2i + 1, and a set bit means "composite".
static List<uint> SievePrimes()
{
    int sieveBits = (int)(Limit / 2 + 1);
    ulong[] composite = new ulong[(sieveBits + 63) / 64];

    // Cross out odd multiples of each odd prime p, starting at p * p.
    for (ulong p = 3; p * p <= Limit; p += 2)
    {
        int pi = (int)(p >> 1);
        if (((composite[pi >> 6] >> (pi & 63)) & 1) != 0) continue; // p is composite
        // Consecutive odd multiples of p are p apart in bit index.
        for (int m = (int)((p * p) >> 1); m < sieveBits; m += (int)p)
            composite[m >> 6] |= 1UL << (m & 63);
    }

    // 2 is the only even prime; then every odd number whose bit is still 0.
    var primes = new List<uint>(78498) { 2 };
    for (int i = 1; i < sieveBits; i++)
    {
        uint n = 2u * (uint)i + 1;
        if (n > Limit) break;
        if (((composite[i >> 6] >> (i & 63)) & 1) == 0) primes.Add(n);
    }
    return primes;
}

string which = args.Length > 0 ? args[0] : "";
var result = new List<uint>(78498);

// Time the computation only; printing is done afterwards.
long t0 = Stopwatch.GetTimestamp();
switch (which)
{
    case "scalar":
        for (uint n = 0; n <= Limit; n++)
            if (IsPrimeScalar(n)) result.Add(n);
        break;
    case "avx2":
        for (uint n = 0; n <= Limit; n++)
            if (IsPrimeAvx2(n)) result.Add(n);
        break;
    case "sieve":
        result = SievePrimes();
        break;
    default:
        Console.Error.WriteLine("usage: prime scalar|avx2|sieve");
        return 2;
}
double ms = Stopwatch.GetElapsedTime(t0).TotalMilliseconds;

var sb = new StringBuilder(result.Count * 7);
foreach (uint p in result) sb.Append(p).Append('\n');
using (var stdout = Console.OpenStandardOutput())
{
    byte[] bytes = Encoding.ASCII.GetBytes(sb.ToString());
    stdout.Write(bytes, 0, bytes.Length);
}

Console.Error.WriteLine(string.Create(System.Globalization.CultureInfo.InvariantCulture,
    $"Found {result.Count} primes from 0 to {Limit} in {ms:F3} ms"));
return 0;
