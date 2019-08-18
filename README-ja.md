# `seiran` 擬似乱数生成器

[English version is here.](README.md)

LFSRベースの新しい擬似乱数生成器を提案する。

## 最小実装 (C言語)

```c
uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (-k & 63)); }
// state は {0, 0} 以外の値で初期化すること
uint64_t next(uint64_t state[2]) {
	uint64_t s0 = state[0], s1 = state[1];
	uint64_t result = rotl((s0 + s1) * 9, 29) + s0;
	state[0] = s0 ^ rotl(s1, 29);
	state[1] = s0 ^ s1 << 9;
	return result;
}
```

詳しくは `seiran128.c` を参照すること。

## 利点

1. 64 bit 環境で高速に実行可能。
	* 64 bit 版メルセンヌツイスタの約 3.7 倍の速度で実行可能。
1. 移植性が高い。実装が容易である。
	* 128 bit 乗算など、環境・言語依存の命令を使用していない。
1. 周期は 2^128 - 1 。数学的に証明可能。
	* ゲーム・シミュレーション用途では十分である。
1. 出力は 1 次元均等分布する。64 bit 整数のすべての値がほぼ等確率に出力される。
1. 低品質のビット位置が存在しない。
	* 線形合同法のように、下位ビットの乱数性が低い問題が存在しない。
1. 空間利用効率が良い。
	* 128 bit == 16 byte の内部状態領域のみを用いる。
1. 多くの強力な乱数性テストにパスする。  
	(以下では、rev は出力のビット順を逆転させたもの、std は出力をそのまま用いたものを表す。)
	* [PractRand](http://pracrand.sourceforge.net/) v0.94 expanded extra ( `-tf 2 -te 1` ) 32TB: no anomalies in 2417 test result(s)
	* [Hamming-weight dependencies](http://prng.di.unimi.it/hwd.php) 1PB: p = 0.581
	* [TestU01](http://simul.iro.umontreal.ca/testu01/tu01.html) v.1.2.3 BigCrush std/rev: すべてのテストにおいて p in [1e-10, 1 - 1e-10]
	* [gjrand](http://gjrand.sourceforge.net/) v4.2.1.0
		* testunif 10T ( `--ten-tera` ): p = std 0.369 / rev 0.276
		* testfunif 1T ( `--tera` ): p = std 0.574 / rev 0.145
1. ジャンプ機能を持ち、並列実行が容易。
	* 2^64 回の `next()` 呼び出しと同等の操作が定数時間で可能。
	* 重複しない、長さ 2^64 のストリームが 2^64 本使用できることになる。

## 欠点

1. 暗号論的擬似乱数生成器ではない。
	* 暗号化・鍵生成など、セキュリティ強度を要する場合には絶対に使用してはならない。
	* 3 つの連続した 64 bit の出力から、[Z3 ソルバ](https://github.com/Z3Prover/z3) を用いて内部状態の復元を試みたところ、数分で復元に成功した。
1. 32 bit ワードのバリアントが存在しない。
1. 新しいため、他者による調査がなされていない。

## 比較

主要な 64bit 出力の擬似乱数生成器との比較を以下に示す。

|名前|周期|サイズ(byte)|均等分布次元|失敗するテスト|速度(64bit/ns)|
|:---|---:|---:|---:|:---|---:|
|[`sfc64`](http://pracrand.sourceforge.net/)|> 2^64|32|0|-|1.21|
|👉 `seiran128`|2^128 - 1|16|1|-|1.20|
|[`xoroshiro128+`](http://xoshiro.di.unimi.it/xoroshiro128plus.c)|2^128 - 1|16|1|BRank, hwd|1.13|
|[`shioi128`](https://github.com/andanteyk/prng-shioi)|2^128 - 1|16|1|-|1.00|
|[`xoshiro256**`](http://xoshiro.di.unimi.it/xoshiro256starstar.c)|2^256 - 1|32|4|-|0.99|
|[`lehmer128`](https://web.archive.org/web/20180609040734/http://xoshiro.di.unimi.it/lehmer128.c) (線形合同法)|2^126|16|1|TMFn|0.74|
|[`splitmix`](http://xoshiro.di.unimi.it/splitmix64.c)|2^64|8|1|-|0.68|
|[`pcg64_xsh_rr`](http://www.pcg-random.org/)|2^128|16|1|-|0.38|
|[`mt19937_64`](http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/VERSIONS/C-LANG/mt19937-64.c) (Mersenne Twister)|2^19937 - 1|2500|311|BRank|0.32|
|[`tinymt64`](http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/TINYMT/index-jp.html)|2^127 - 1|16|1|BRank, hwd|0.24|

速度測定では [xoshiro/xoroshiro で用いられたハーネス](http://xoshiro.di.unimi.it/harness.c) を使用し、`Intel(R) Core(TM) i7-7700HQ CPU @ 2.80GHz / gcc 7.3.0` 環境下で実施した。  
環境や状況によって速度は異なる可能性がある。

## ライセンス
[Public Domain (CC0)](https://creativecommons.org/publicdomain/zero/1.0/)
