# PGPG

**P**ipeline **G**enerator for **P**rogrammable **G**RAPE — FPGA ベース計算エンジン向けパイプライン設計の自動生成ツール

[![Paper](https://img.shields.io/badge/Paper-arXiv:astro--ph%2F0703182-blue)](https://arxiv.org/abs/astro-ph/0703182)
[![PASJ](https://img.shields.io/badge/PASJ-57%285%29%20799--813-green)](https://academic.oup.com/pasj/article/57/5/799/2948924)

---

## 概要

**PGPG** は、**PROGRAPE**（Programmable GRAPE）やその他の **FBCE**（FPGA-Based Computing Engine）向けに、パイプライン処理器の低レベル設計と通信ソフトウェアを**自動生成**するソフトウェアです。

従来、1 人年以上を要していた設計作業を、**PGDL**（PGPG Description Language）による簡潔な記述に置き換えます。PGDL で記述した高レベル設計から、以下を**一括生成**します。

| 出力 | 説明 |
|------|------|
| **VHDL** | パイプライン論理・制御論理・I/O 論理 |
| **C ソース** | ビットレベルエミュレータ |
| **C ソース** | ホスト用インターフェースプログラム |

エミュレータと実ハードウェア用ドライバは同じ API を持つため、リンクするオブジェクトを切り替えるだけで、アプリケーションを変更せずに検証・実行できます。

---

## 主な用途

- **N 体シミュレーション**（重力計算、SPH 流体シミュレーションなど）
- 粒子間相互作用のパイプライン化
- 分子動力学（MD）、境界要素法（BEM）などへの拡張計画

---

## 設計の特徴

| 項目 | 内容 |
|------|------|
| **数値表現** | 固定小数点（fix/ufix）、LNS: Logarithmic Number System (対数形式)（log）、浮動小数点（FLP）の併用 |
| **出力形式** | VHDL、C（エミュレータ・インターフェース） |
| **対象 FPGA** | Altera（APEX20K, Cyclone, Stratix 等） |
| **ハードウェア** | PROGRAPE-2 対応 |
| **パイプライン** | 可変段数、マルチパイプ（NPIPE）、仮想マルチパイプ（NVMP）対応 |

---

## クイックスタート

### ビルド

```bash
# VHDL 生成（pgpgvt: パイプトップ / pgpgvm: モジュール）
cd src/pgpg1.0/src_h/v
make pgpgvt   # pg_pipe.vhd を出力
make pgpgvm   # pg_module.vhd を出力

# C演算器エミュレータトランスパイラ（pgpgcm）
cd src/pgpg1.0/src_h/cm
make pgpgcm

# C エミュレータ生成（pgpgct）
# bin/pgpgct をビルド後、使用
```

### 実行例

```bash
# VHDL 生成
./pgpgvt list/list.sph.cpp
# または
./pgpgvm list/list.sph.cpp

# C演算器エミュレータトランスパイラ
./pgpgcm list/list.sph.cpp
# 出力: pg_module.c
```

### コンパイルフロー（bin/pgpgc）

```bash
cpp list/list.sph list/list.sph.cpp   # プリプロセス
pgpgct list/list.sph.cpp             # エミュレータ (pg_pipe.c)
pgpgcm list/list.sph.cpp             # 演算器エミュレータ (pg_module.c)
```

---

## PGDL の概要

PGDL はパイプライン処理器の記述に特化した高レベル言語です。プログラムは次の 4 セクションで構成されます。

1. **マクロ宣言** — `#define` でスケール等を定義
2. **ジェネリック宣言** — `/NPIPE`, `/NVMP` でパイプ数・仮想マルチパイプ数を指定
3. **インターフェース宣言** — `/JPSET`, `/IPSET`, `/FOSET` でレジスタ・メモリ・API を定義
4. **パイプライン記述** — `pg_*` モジュールで演算の流れを記述

### 人工例（論文 Figure 5）

目標関数: \( f_i = \sum_j a_i a_j \)

```pgdl
#define ascale (pow(2.0,20.0))
#define fscale (1.0/(ascale*ascale))
/NVMP 1;
/NPIPE 2;
/JPSET iaj,aj[],log,17,8,ascale;
/IPSET iai,ai[],log,17,8,ascale;
/FOSET sfij,f[],fix,64,fscale;
pg_log_muldiv(MUL,iaj,iai,aij,17,1);
pg_conv_ltof(aij,fij,17,8,64,1);
pg_fix_accum(fij,sfij,64,64,1);
```

### 重力パイプラインの API 例

```c
void force(double x[][3], double m[], double eps2, double a[][3], int n);
```

---

## システムアーキテクチャ

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        PGPG コンパイルフロー                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   PGDL ──┬──▶ pgpgct ──▶ pg_pipe.c (エミュレータ)                           │
│          ├──▶ pgpgcm ──▶ pg_module.c (演算器エミュレータ)                    │
│          └──▶ pgpgvt/vm ──▶ *.vhd (VHDL)                                    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### ディレクトリ構成

```
src/pgpg1.0/
├── bin/
│   ├── pgpgc          # スクリプト: pgpgct → pgpgcm
│   └── pgpgct         # C エミュレータトランスパイラ
├── src_h/
│   ├── v/             # VHDL 生成 (pgpgvt, pgpgvm)
│   ├── cm/            # C演算器エミュレータトランスパイラ (pgpgcm)
│   └── ct/            # C エミュレータトランスパイラ (pgpgct)
├── list/              # サンプル PGDL ファイル
│   ├── list.sph       # SPH パイプライン例
│   └── list.sph.cpp   # プリプロセス後
└── qtest/             # Quartus テスト用
```

---

## 性能（論文より）

### 重力パイプライン（Stratix）

| モデル | 座標 | 内部（仮数） | アキュム | fmax | LE |
|--------|------|--------------|----------|------|-----|
| G3 | 20bit fix | 14(5)bit log | 56bit fix | 191 MHz | 2369 |
| G5 | 32bit fix | 17(8)bit log | 64bit fix | 182 MHz | 3021 |
| G5+ | 32bit fix | 20(11)bit log | 64bit fix | 142 MHz | 5082 |

Stratix EP1S20 に 5 本の G5 パイプラインを 180 MHz で実装可能。元の GRAPE-5 チップ（2 本、80 MHz）と比べ約 **5 倍**の性能。

---

## ドキュメント

| ドキュメント | 内容 |
|--------------|------|
| [doc/design.md](doc/design.md) | 設計仕様書（PGDL、モジュール、ビルド手順など） |
| [doc/ChangeLog](doc/ChangeLog) | 変更履歴 |
| [paper/README.md](paper/README.md) | 技術論文の解説（時代背景、先行研究、GRAPE/PROGRAPE-1/PGPG の系譜） |
| [faq/pgpg-influence-and-history.md](faq/pgpg-influence-and-history.md) | PGPG の影響、GPU・TPU との思想的共通点、GRAPE が GPU に負けた理由、日本が AI チップで遅れた背景 |
| [paper/PGPG.pdf](paper/PGPG.pdf) | 技術論文（PDF） |

---

## 参考文献

> **PGPG: An Automatic Generator of Pipeline Design for Programmable GRAPE Systems**  
> Tsuyoshi Hamada, Toshiyuki Fukushige, Junichiro Makino  
> *Publications of the Astronomical Society of Japan*, Volume 57, Issue 5, 25 October 2005, Pages 799–813  
> https://doi.org/10.1093/pasj/57.5.799

- [arXiv:astro-ph/0703182](https://arxiv.org/abs/astro-ph/0703182)
- [PASJ HTML](https://academic.oup.com/pasj/article/57/5/799/2948924)
- [PROGRAPE 公式サイト](http://progrape.jp) — CGI による PGDL から VHDL/エミュレータ/インターフェース生成
- [Chamomile Scheme](https://arxiv.org/abs/astro-ph/0703100)（Hamada & Iitaka 2007）— PGPG と同時期・同一著者による GPU 向け N 体シミュレーション、CUNBODY-1

---

## 貢献者

- **Tsuyoshi Hamada**（東京大学・総合文化研究科）
- **Toshiyuki Fukushige**（東京大学・総合文化研究科）
- **Junichiro Makino**（東京大学・理学系研究科天文学専攻）

---

## ライセンス

本プロジェクトのライセンスについては、リポジトリの LICENSE ファイルを参照してください。
