# PGPG 技術論文の解説

**PGPG: An Automatic Generator of Pipeline Design for Programmable GRAPE Systems**  
Tsuyoshi Hamada, Toshiyuki Fukushige, Junichiro Makino  
*Publications of the Astronomical Society of Japan*, Vol. 57, No. 5 (2005), pp. 799–813  
arXiv: [astro-ph/0703182](https://arxiv.org/abs/astro-ph/0703182)

> **関連論文**: PGPG の共著者 Hamada、Fukushige、Makino は、[PROGRAPE-1](https://arxiv.org/abs/astro-ph/9906419)（Hamada et al. 1999/2000）で **Programmable GRAPE** の概念を初めて実装した。PROGRAPE-1 の開発で露呈した「パイプライン設計に 1 人年以上」という生産性の壁が、PGPG 開発の直接の動機となった。さらに、**PGPG と同時期**（2007 年 3 月）に、同じ Hamada らは [Chamomile Scheme](https://arxiv.org/abs/astro-ph/0703100)（Hamada & Iitaka 2007）で **GPU 向け N 体シミュレーション**を発表している。研究の系譜：GRAPE → PROGRAPE-1 → **PGPG**（FPGA）∥ **Chamomile Scheme**（GPU）。

---

## 1. 時代背景

### 1.1 天体 N 体シミュレーションの計算需要

1990年代前半から、惑星系形成、球状星団、銀河、銀河団、大規模構造など、様々な天体システムの形成・進化を扱う **N 体シミュレーション**が広く利用されるようになった。重力は長距離相互作用であり、単純なスキームでは 1 タイムステップあたりの計算コストが O(N²) となる。Barnes-Hut ツリー法（Barnes & Hut 1986）などで O(N log N) に削減できるものの、係数が大きく、**粒子間相互作用の計算が全体のボトルネック**となっていた（[Ebisuzaki et al. 1993](https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1993PASJ...45..269E&defaultprint=YES&filetype=.pdf)）。

SPH（Smoothed Particle Hydrodynamics）流体シミュレーションでも、粒子間の短距離相互作用の計算コストが高く、1 粒子あたり約 50 粒子との相互作用を計算する必要があり、重力計算より複雑だった。

### 1.2 専用ハードウェアの台頭：GRAPE プロジェクト

このような背景から、**GRAPE**（GRAvity piPE）プロジェクト（Sugimoto et al. 1990）が始まった。重力相互作用の計算は比較的単純（1 ペアあたり約 20〜30 浮動小数点演算相当）であり、全粒子間の相互作用という並列性が高いため、専用パイプラインハードウェアによる加速が有望だった。

[Ebisuzaki et al. (1993)](https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1993PASJ...45..269E&defaultprint=YES&filetype=.pdf) の **GRAPE プロジェクト概要**によれば、GRAPE は次の基本アーキテクチャを採用している。

- **ホスト計算機**が粒子の位置を GRAPE に送信
- **GRAPE** が重力を計算し、結果をホストに返す
- **ホスト**が受け取った力で粒子の軌道を積分

この「ホスト＋専用アクセラレータ」構成は、PGPG が生成する PROGRAPE システムと同一である。GRAPE はユーザーから見れば「ハードウェアサブルーチン」として扱える（Makino & Funato 1993）。

#### GRAPE の系譜（Ebisuzaki et al. 1993 より）

GRAPE は **低精度型**（奇数番号）と **高精度型**（偶数番号）に分かれる。

| 型 | 用途 | マシン | 特徴 |
|----|------|--------|------|
| 低精度 | 無衝突系（銀河、銀河団） | GRAPE-1, 1A, 3, 3A | 対数表現、固定小数点、開発コスト低 |
| 高精度 | 衝突系（球状星団、惑星系） | GRAPE-2, 2A, 4 | 浮動小数点、近接遭遇を正確に計算 |

- **GRAPE-1**（1989）: ワイヤラップ、8 MHz、240 Mflops、約 5 ヶ月で開発、約 30 万円。**対数表現**（8 bit）を採用し、乗除算を加算に変換。
- **GRAPE-1A**（1990）: ツリー法・SPH 対応、近傍粒子リスト、VME バス。
- **GRAPE-3**（1991）: 48 個のカスタム LSI（GRAPE チップ）、14.4 Gflops ピーク、10 Gflops 実効。対数表現 12 bit。
- **GRAPE-2A**（1992）: 分子動力学・天体両用。補間テーブルで van der Waals、Coulomb、Ewald 法の実空間成分を計算可能。
- **GRAPE-4**（計画）: 約 1,600 パイプライン、ピーク約 1 Tflops、1995 年完成予定、約 1 億円。

GRAPE システムは、汎用スーパーコンピュータと比べて **価格性能比で 100〜10,000 倍** 優れていた（Ebisuzaki et al. 1993）。

#### GRAPE-4 の実績と GRAPE-6

[Fukushige & Makino (1996)](https://arxiv.org/abs/astro-ph/9612090) では、**GRAPE-4** による銀河形成 N 体シミュレーション（78 万粒子、185 時間）で平均 332 Gflops を達成し、1,269 個のパイプライン処理器でピーク 663 Gflops、価格性能比 4,600 ドル/Gflops を実現した。この成果は **1996 年ゴードン・ベル賞**を受賞した。

**GRAPE-6** は約 5 億円で 64 Tflops のピーク性能を達成し、当時の Earth Simulator（40 Tflops）や ASCI-Q（30 Tflops）と同等以上の性能を、桁違いに安いコストで実現した。**2003 年ゴードン・ベル賞（特別賞）**を受賞している。

### 1.3 PROGRAPE-1：Programmable GRAPE の初実装（1999）

GRAPE の課題は、**1/r ポテンシャル以外の相互作用を扱えない**ことだった。GRAPE-2A は補間テーブルで分子動力学にも対応したが（Ito et al. 1993）、重力と SPH の両方を同一ハードウェアで扱う設計は困難だった。従来の GRAPE では、SPH の近傍粒子探索は GRAPE 上で行えたが、**実際の SPH 相互作用の計算はホスト計算機**で行われており、これがボトルネックとなっていた（Yokono et al. 1999）。

[**Hamada et al. (1999/2000)**](https://arxiv.org/abs/astro-ph/9906419) は、**PROGRAPE-1**（PROgrammable GRAPE-1）を開発し、このアプローチを **Programmable GRAPE** と名付けた。PROGRAPE-1 は、従来のハードワイヤードな GRAPE と異なり、**FPGA**（Field-Programmable Gate Array）を処理要素として用いる。FPGA の論理は再構成可能であるため、**重力に加えて van der Waals 力、SPH の流体力学相互作用など、様々な相互作用**を同一ハードウェアで計算できる。

#### PROGRAPE-1 の仕様と成果（Hamada et al. 1999 より）

| 項目 | 内容 |
|------|------|
| **ハードウェア** | Altera EPF10K100 FPGA × 2（各 10 万ゲート） |
| **実装パイプライン** | GRAPE-3 と同様の重力相互作用パイプライン |
| **数値表現** | 減算 20 bit 固定小数点、除算・平方根 14 bit 対数（7 bit 指数・5 bit 仮数）、累積 56 bit 固定小数点 |
| **動作** | 1 チップに 1 パイプライン、16 MHz 動作 |
| **性能** | 0.96 Gflops 相当（重力計算） |

PROGRAPE-1 は、**粒子ベースシミュレーションにおいて重力以外の相互作用（SPH など）の計算コストが高い場合**に特に有用であると結論づけている。

#### PROGRAPE-1 が露呈した課題 → PGPG の動機

PROGRAPE-1 の開発を通じて、**パイプラインの詳細設計、制御論理、ホスト通信、データ変換ライブラリ**の開発に膨大な工数がかかることが明らかになった。PGPG 論文によれば、比較的単純な重力パイプラインの実装でも **1 人年以上**を要した。この設計生産性の壁が、**PGPG**（高レベル記述から VHDL・エミュレータ・インターフェースを一括生成するツール）開発の**直接の動機**となった。

---

## 2. 先行研究

### 2.1 数値アルゴリズム

| 研究 | 内容 |
|------|------|
| Barnes & Hut (1986) | Barnes-Hut ツリー法による O(N log N) 重力計算 |
| Greengard & Rokhlin (1987) | FMM（高速多重極展開法） |
| Lucy (1977), Gingold & Monaghan (1977) | SPH 流体シミュレーション |

### 2.2 専用ハードウェア・GRAPE

| 研究 | 内容 |
|------|------|
| Sugimoto et al. (1990) | GRAPE プロジェクト開始（Nature） |
| [Ebisuzaki et al. (1993)](https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1993PASJ...45..269E&defaultprint=YES&filetype=.pdf) | GRAPE プロジェクト概要（GRAPE-1〜4、アーキテクチャ、低/高精度型） |
| Ito et al. (1993), Fukushige et al. (1996) | 分子動力学用 GRAPE（GRAPE-2A 系） |
| [Fukushige & Makino (1996)](https://arxiv.org/abs/astro-ph/9612090) | GRAPE-4 による銀河形成 N 体シミュレーション（78 万粒子、332 Gflops、1996 年ゴードン・ベル賞） |
| Makino & Taiji (1998) | GRAPE システムの体系化 |
| Kawai et al. (2000) | GRAPE-5 パイプライン設計（対数表現の採用） |

### 2.3 再構成可能計算・PROGRAPE

| 研究 | 内容 |
|------|------|
| Buell et al. (1996) | Splash-1, Splash-2 |
| Kim et al. (1995) | FPGA による粒子シミュレーション |
| [**Hamada et al. (1999/2000)**](https://arxiv.org/abs/astro-ph/9906419) | **PROGRAPE-1**：FPGA ベース Programmable GRAPE の初実装。2× Altera EPF10K100、GRAPE-3 相当パイプライン、0.96 Gflops。重力・van der Waals・SPH への拡張可能性を実証。**PGPG の直接の前身** |
| Spurzem et al. (2002) | FPGA を用いた N 体シミュレーション |

### 2.4 高レベル設計手法

当時、デジタル信号処理分野では MATLAB などから HDL を生成するツールが存在した。また、**システムレベル設計**（SLDL）が広まり、SpecC、SystemC、Handel-C などが C/C++ 系でハードウェアを記述する手段として使われ始めていた。しかし、これらは依然として詳細なハードウェア記述を必要とし、PGPG の目指す「パイプラインの高レベル記述から全設計を自動生成する」というアプローチとは異なっていた。

### 2.5 関連の強い並行研究：Chamomile Scheme（PGPG と同時期）

PGPG 論文の arXiv 投稿（2007 年 3 月 8 日）の **2 日前**、同じ筆頭著者（Hamada）から [**Chamomile Scheme**](https://arxiv.org/abs/astro-ph/0703100)（Hamada & Iitaka 2007）が発表された。これは **GPU** 上での重力 N 体シミュレーション最適化アルゴリズムであり、CUNBODY-1 ライブラリとして実装された。PGPG が FPGA 設計の生産性を扱う一方で、Chamomile Scheme は GPU の共有メモリ・スレッド並列性を活用し、GRAPE と同様の「粒子間相互作用の並列化」を汎用 GPU で実現した。**同一研究グループによる FPGA と GPU の並行研究**として、関連性が極めて強い。

---

## 3. 論文内容

### 3.1 概要と PROGRAPE-1 からの発展

PGPG は、**PGDL**（PGPG Description Language）で記述された高レベル設計から、以下を**一括自動生成**するソフトウェアである。

- **VHDL**：パイプライン論理、制御論理、I/O 論理
- **C ソース**：ビットレベルエミュレータ
- **C ソース**：ホスト用インターフェースプログラム

エミュレータと実ハードウェア用ドライバは同じ API を持つため、リンクするオブジェクトを切り替えるだけで、アプリケーションを変更せずに検証・実行できる。

[PROGRAPE-1](https://arxiv.org/abs/astro-ph/9906419) では、これらすべてを**手作業で**開発する必要があった。PGPG は、PROGRAPE-1 で実証された「FPGA 上の GRAPE 型パイプライン」というアーキテクチャを維持しつつ、**設計記述を PGDL の数十行に集約**することで、1 人年以上かかっていた作業を大幅に短縮する。

### 3.2 GRAPE アーキテクチャとの対応

PGPG が生成するハードウェアモデルは、[Ebisuzaki et al. (1993)](https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1993PASJ...45..269E&defaultprint=YES&filetype=.pdf) で述べられた GRAPE の基本構造を継承している。

| GRAPE（Ebisuzaki et al. 1993） | PGPG 生成ハードウェア |
|-------------------------------|------------------------|
| 位置 \(x_j\) をメモリに格納 | j 粒子メモリ |
| 力の計算点 \(x_i\) をレジスタに保持 | i 粒子レジスタ |
| パイプラインで力 \(F_i\) を計算 | パイプライン単位 |
| 力の累積 | 力アキュムレータ |

ホストが粒子位置を送り、専用ハードウェアが力を返すという構成は、GRAPE-1 以来一貫している。

### 3.3 PGDL 設計フロー

従来の 5 ステップ（目標関数仕様 → エミュレータ → ハードウェア設計 → インターフェースプログラム → アプリケーション結合）を、**PGDL ファイル 1 つ**の記述に集約する。

PGDL プログラムは次の 4 セクションで構成される。

1. **マクロ宣言**：`#define` でスケール等を定義
2. **ジェネリック宣言**：`/NPIPE`、`/NVMP` でパイプ数・仮想マルチパイプ数を指定
3. **インターフェース宣言**：`/JPSET`、`/IPSET`、`/FOSET` でレジスタ・メモリ・API を定義
4. **パイプライン記述**：`pg_*` モジュールで演算の流れを記述

### 3.4 数値表現と GRAPE との継承

PGPG 1.0 は **固定小数点**（fix/ufix）と **対数表現**（log）をサポートする。対数表現では乗除算が加減算になるため、ハードウェアの複雑さと遅延を削減できる。この設計思想は **GRAPE-1** 以来の伝統であり（Ebisuzaki et al. 1993）、GRAPE-3 では 12 bit、[PROGRAPE-1](https://arxiv.org/abs/astro-ph/9906419) では 14 bit（7 bit 指数・5 bit 仮数）の対数表現、GRAPE-5 では 17(8) bit が用いられた。PGPG は GRAPE-5 と同様の設計を採用している。

### 3.5 重力パイプラインの実例

重力加速度 \( f_i = \sum_j m_j \boldsymbol{r}_{ij} / (r_{ij}^2 + \varepsilon^2)^{3/2} \) を PGDL で記述し、GRAPE-3 / GRAPE-5 と同等の設計を自動生成した。Stratix 上では、GRAPE-5 と同様のパイプラインを 5 本、180 MHz で実装可能であり、元の GRAPE-5 チップ（2 本、80 MHz）と比べ約 **5 倍**の性能を達成した。

### 3.6 今後の拡張計画（論文当時）

- SPH パイプライン、境界要素法（BEM）のサポート
- 浮動小数点演算モジュールの追加
- Xilinx FPGA への対応

---

## 4. その後（2026年現在）の研究開発に与えた影響

### 4.1 高レベル合成（HLS）の普及

PGPG は「高レベル記述からハードウェアを自動生成する」というアプローチを、**天体・粒子シミュレーションというドメイン**に特化して実現した。2010 年代以降、**HLS**（High-Level Synthesis）が FPGA 設計の主流となり、C/C++、SystemC、MATLAB などから RTL を生成する手法が広く普及した。PGPG は、**ドメイン特化型 DSL** によるハードウェア生成の先駆的な例として位置づけられる。

### 4.2 ドメイン特化言語（DSL）による FPGA アクセラレータ設計

2020 年代には、**Spatial**（Stanford）、**Graphitron**、**Scotch** など、**DSL から FPGA アクセラレータを自動生成**する研究が活発になっている。PGPG の「パイプラインの高レベル記述から VHDL、エミュレータ、インターフェースを一括生成する」という設計思想は、これらの DSL と共通する。PGPG は、**ドメイン特化による設計生産性の向上**という方向性を、早期に示した事例として評価できる。

### 4.3 プロジェクトの継続と発展

本リポジトリ（`pgpg`）では、PGPG のソースコードが維持されており、**SPH パイプライン**などへの拡張が進められている。設計仕様書（`doc/design.md`）には、浮動小数点（FLP）のサポート、複数 FPGA の対応など、論文の拡張計画を踏まえた発展が記述されている。

### 4.4 Chamomile Scheme と GPU による N 体シミュレーションの変遷

PGPG 論文が arXiv に投稿された **2007 年 3 月**、同じ研究グループから **2 日違い**で別の論文が発表された。[**Hamada & Iitaka (2007)**](https://arxiv.org/abs/astro-ph/0703100) の **Chamomile Scheme** である。これは、**プログラム可能な GPU**（NVIDIA GeForce 8800 GTX）上で重力 N 体シミュレーションを最適化するアルゴリズムであり、ライブラリ **CUNBODY-1** として実装された。

#### Chamomile Scheme の設計思想と PGPG との接点

Chamomile Scheme は、当時の GPU の制約（16 KB × 16 の共有メモリ、ブロードキャスト機構なし、単精度 500 Gflop/s の浮動小数点演算）に最適化されている。GRAPE が専用パイプラインで「1 粒子から多粒子への力の並列計算」を実現したのと同様に、Chamomile Scheme は **GPU の共有メモリとスレッド並列性**を活用し、粒子間相互作用の計算を効率化した。PGPG の著者 Hamada が両方の論文の筆頭著者であり、**FPGA 設計の自動化（PGPG）** と **GPU アルゴリズムの最適化（Chamomile Scheme）** を並行して推進していたことがわかる。

| 項目 | Chamomile Scheme（Hamada & Iitaka 2007） |
|------|------------------------------------------|
| **ハードウェア** | NVIDIA GeForce 8800 GTX |
| **ライブラリ** | CUNBODY-1 |
| **性能** | 2,048 粒子で 173 Gflop/s、131,072 粒子で 256 Gflop/s |
| **設計** | 共有メモリの制約を考慮したドメイン特化アルゴリズム |

#### GPU の台頭と FPGA の役割分担

Chamomile Scheme 以降、**GPU** による N 体シミュレーションが急速に普及した。Sapporo（GRAPE 互換 API を GPU で実装）、CUDA ベースのツリーコード、FMM などが開発され、Hamada らは 2010 年に DEGIMA GPU クラスタで 190 Tflops を達成し、SC'09 ゴードン・ベル賞（価格性能部門）を受賞している。GPU は、**コスト・入手性・プログラミング容易性**の面で GRAPE や FPGA を上回り、天体 N 体シミュレーションの主流となった。

一方、**FPGA** は低電力・高スループットが求められるエッジ AI、データセンターのアクセラレータ、ストリーミング処理などで活躍している。PGPG が示した「**ドメイン特化による設計の簡素化**」というアプローチは、FPGA や ASIC の設計生産性を高める現代的な HLS や DSL の文脈で、再評価される価値がある。また、Chamomile Scheme に代表される GPU アルゴリズム設計も、GRAPE/PROGRAPE で培った「粒子相互作用のパイプライン化」の知見を継承しており、**専用ハードウェア（GRAPE）→ 再構成可能（PROGRAPE/PGPG）→ 汎用並列（GPU）** という技術の連続性が読み取れる。

### 4.5 学術的意義

Ebisuzaki et al. (1993) は、GRAPE を「ユーザーから見ればハードウェアサブルーチン」と表現した。[PROGRAPE-1](https://arxiv.org/abs/astro-ph/9906419) は、その「サブルーチン」を **FPGA 上で再構成可能**にしたが、設計には 1 人年以上を要した。PGPG は、**天体物理学の専門家がハードウェア設計の詳細を学ばずに**、その「サブルーチン」を FPGA 上で自作・カスタマイズできるようにすることを目指した。この「ドメイン専門家の知識」と「ハードウェア設計の詳細」の分離は、**ドメイン特化アーキテクチャ（DSA）** や **DSL の設計**において、今日も重要な設計原則として受け継がれている。

### 4.6 研究の系譜（まとめ）

```
GRAPE (1990-)        → 専用ハードウェア、重力のみ
    ↓
PROGRAPE-1 (1999)    → FPGA で再構成可能に。重力・SPH・van der Waals へ拡張可能
    ↓                 設計生産性の壁（1 人年以上）を露呈
PGPG (2005)          → 高レベル記述から一括生成。設計工数を大幅短縮
    │
    ├── FPGA 系：HLS、DSL、PROGRAPE-2 など
    │
    └── GPU 系（並行研究、同一著者）：
        Chamomile Scheme (2007) → CUNBODY-1、256 Gflop/s
            ↓
        Sapporo、DEGIMA (2009)、ツリーコード/FMM on GPU
            ↓
        N 体シミュレーションの主流は GPU へ
```

---

## 5. 参考文献

### 論文本体

- Hamada, T., Fukushige, T., & Makino, J. (2005). PGPG: An Automatic Generator of Pipeline Design for Programmable GRAPE Systems. *Publications of the Astronomical Society of Japan*, 57(5), 799–813. [DOI](https://doi.org/10.1093/pasj/57.5.799)
- arXiv: [astro-ph/0703182](https://arxiv.org/abs/astro-ph/0703182)

### 主要参考文献

- Barnes, J., & Hut, P. (1986). Barnes-Hut ツリー法
- Sugimoto, D., et al. (1990). GRAPE プロジェクト開始
- [Ebisuzaki, T., Makino, J., Fukushige, T., Taiji, M., Sugimoto, D., Ito, T., & Okumura, S. K. (1993). GRAPE Project: An Overview. *PASJ*, 45, 269–278](https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1993PASJ...45..269E&defaultprint=YES&filetype=.pdf)
- [Fukushige, T., & Makino, J. (1996). N-body Simulation of Galaxy Formation on GRAPE-4 Special-Purpose Computer. arXiv:astro-ph/9612090](https://arxiv.org/abs/astro-ph/9612090)
- Makino, J., & Taiji, M. (1998). GRAPE システム
- [**Hamada, T., Fukushige, T., Kawai, A., & Makino, J. (1999/2000). PROGRAPE-1: A Programmable, Multi-Purpose Computer for Many-Body Simulations. *PASJ*, 52, 943. arXiv:astro-ph/9906419](https://arxiv.org/abs/astro-ph/9906419) — **PGPG の直接の前身。FPGA による Programmable GRAPE の初実装**
- Kawai, A., et al. (2000). GRAPE-5 パイプライン
- [**Hamada, T., & Iitaka, T. (2007). The Chamomile Scheme: An Optimized Algorithm for N-body simulations on Programmable Graphics Processing Units. arXiv:astro-ph/0703100](https://arxiv.org/abs/astro-ph/0703100) — **PGPG と同時期・同一著者。GPU 向け N 体シミュレーション、CUNBODY-1**

### 関連リソース

- [PROGRAPE 公式サイト](http://progrape.jp)
- [CUNBODY-1（Chamomile Scheme 実装）](https://github.com/thamada/cunbody1)
- [本リポジトリ README](../README.md)
- [設計仕様書](../doc/design.md)
