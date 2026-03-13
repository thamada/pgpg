# PGPG 技術論文の解説

**PGPG: An Automatic Generator of Pipeline Design for Programmable GRAPE Systems**  
Tsuyoshi Hamada, Toshiyuki Fukushige, Junichiro Makino  
*Publications of the Astronomical Society of Japan*, Vol. 57, No. 5 (2005), pp. 799–813  
arXiv: [astro-ph/0703182](https://arxiv.org/abs/astro-ph/0703182)

> **関連論文**: PGPG の共著者 Hamada、Fukushige、Makino は、[PROGRAPE-1](https://arxiv.org/abs/astro-ph/9906419)（Hamada et al. 1999/2000）で **Programmable GRAPE** の概念を実装した。PROGRAPE-1 は[1998 年日本天文学会春季年会](https://www.asj.or.jp/nenkai/archive/1998a/pdf/X02a.pdf)で初発表され、同開発で露呈した「パイプライン設計に 1 人年以上」という生産性の壁が、PGPG の直接の動機となった。PGPG の後継 **PGR** を用いた [Nakasato et al. (2006)](https://arxiv.org/abs/astro-ph/0604295) は **PROGRAPE-3** 上で SPH パイプラインを初実装し、1998 年の目標を達成した。さらに、**PGPG と同時期**（2007 年 3 月）に、同じ Hamada らは [Chamomile Scheme](https://arxiv.org/abs/astro-ph/0703100)（Hamada & Iitaka 2007）で **GPU 向け N 体シミュレーション**を発表している。その発展として、[Hamada et al. (2009)](https://www.cs.umd.edu/class/fall2019/cmsc714/readings/Hamada-nbody.pdf) は **多重ウォーク法**により階層的 N 体法（ツリーコード・FMM）を GPU で初めて高効率に実現し、**2009 年ゴードン・ベル賞（価格性能部門）**で 42 TFlops・124 MFlops/$ を達成した。研究の系譜：GRAPE → PROGRAPE-1（1998/1999）→ **PGPG** → PGR/PROGRAPE-3（SPH）∥ **Chamomile Scheme**（GPU）→ **42 TFlops**（SC09）。

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

#### GRAPE の系譜（Ebisuzaki et al. 1993, [Kawai et al. 1999](https://arxiv.org/abs/astro-ph/9909116) より）

GRAPE は **低精度型**（奇数番号）と **高精度型**（偶数番号）に分かれる。

| 型 | 用途 | マシン | 特徴 |
|----|------|--------|------|
| 低精度 | 無衝突系（銀河、銀河団） | GRAPE-1, 1A, 3, 3A, 5 | 対数表現、固定小数点、開発コスト低 |
| 高精度 | 衝突系（球状星団、惑星系） | GRAPE-2, 2A, 4 | 浮動小数点、近接遭遇を正確に計算 |

- **GRAPE-1**（1989）: ワイヤラップ、8 MHz、240 Mflops、約 5 ヶ月で開発、約 30 万円。**対数表現**（8 bit）を採用し、乗除算を加算に変換。
- **GRAPE-1A**（1990）: ツリー法・SPH 対応、近傍粒子リスト、VME バス。
- **GRAPE-3**（Okumura et al. 1993）: 8 個の GRAPE チップ（各 1 パイプライン、20 MHz）、4.8 Gflops ピーク。対数表現 12 bit。ペアワイズ力の相対誤差約 2%。VME バス。
- **GRAPE-2A**（1992）: 分子動力学・天体両用。補間テーブルで van der Waals、Coulomb、Ewald 法の実空間成分を計算可能。
- **GRAPE-4**（計画）: 約 1,600 パイプライン、ピーク約 1 Tflops、1995 年完成予定、約 1 億円。
- **GRAPE-5**（[Kawai et al. 1999](https://arxiv.org/abs/astro-ph/9909116)）: GRAPE-3 の後継。8 個の G5 チップ（各 2 パイプライン、80 MHz、1 クロックあたり 2 ペア相互作用）、**38.4 Gflops ピーク**。仮想マルチパイプライン（実パイプライン 1 本あたり 6 仮想パイプライン、計 12 仮想パイプライン）でメモリ帯域を削減。PCI バス（VME の約 10 倍の通信速度）。純粋 1/r に加え**任意の cutoff 関数**（オンチップ RAM テーブル）で Ewald 法・P³M 法に対応。セルインデックス法で PP 力の計算コストを削減。ペアワイズ力の精度は GRAPE-3 の**約 10 倍**、動的範囲は **10³ 倍**。対数表現 17(8) bit。メモリ最大 131,072 粒子。128k 体直接和で 14 秒/タイムステップ、100 万体 Barnes-Hut（θ=0.75）で 16 秒/タイムステップ。

GRAPE システムは、汎用スーパーコンピュータと比べて **価格性能比で 100〜10,000 倍** 優れていた（Ebisuzaki et al. 1993）。

#### GRAPE-4 の実績と GRAPE-6

[Fukushige & Makino (1996)](https://arxiv.org/abs/astro-ph/9612090) では、**GRAPE-4** による銀河形成 N 体シミュレーション（78 万粒子、185 時間）で平均 332 Gflops を達成し、1,269 個のパイプライン処理器でピーク 663 Gflops、価格性能比 4,600 ドル/Gflops を実現した。この成果は **1996 年ゴードン・ベル賞**を受賞した。

**GRAPE-5** の開発は 1996 年 5 月に開始され、G5 チップは 1998 年 6 月に完成、プロトタイプボード（4 チップ）は 1998 年 10 月、量産版（8 チップ）は 1999 年 4 月に完成した。NEC 0.5 μm ゲートアレイで製造、約 20 万ゲート、364 ピン PGA、3.3 V、約 10 W/チップ。論文では約 1 Tflops の大規模並列 GRAPE-5 システムの構想も述べられている（Kawai et al. 1999）。

**GRAPE-6** は約 5 億円で 64 Tflops のピーク性能を達成し、当時の Earth Simulator（40 Tflops）や ASCI-Q（30 Tflops）と同等以上の性能を、桁違いに安いコストで実現した。**2003 年ゴードン・ベル賞（特別賞）**を受賞している。

### 1.3 PROGRAPE-1：Programmable GRAPE の初実装（1998/1999）

GRAPE の課題は、**1/r ポテンシャル以外の相互作用を扱えない**ことだった。GRAPE-2A は補間テーブルで分子動力学にも対応したが（Ito et al. 1993）、重力と SPH の両方を同一ハードウェアで扱う設計は困難だった。GRAPE-3 は Plummer ソフトニングのみ対応していたが、[GRAPE-5](https://arxiv.org/abs/astro-ph/9909116) では **任意の cutoff 関数**をオンチップ RAM テーブルで実装し、Ewald 法・P³M 法に対応した（Kawai et al. 1999）。従来の GRAPE では、SPH の近傍粒子探索は GRAPE 上で行えたが、**実際の SPH 相互作用の計算はホスト計算機**で行われており、これがボトルネックとなっていた（Yokono et al. 1999）。

**PROGRAPE-1**（PROgrammable GRAPE-1）は、[日本天文学会 1998 年春季年会](https://www.asj.or.jp/nenkai/archive/1998a/pdf/X02a.pdf)で初めて発表され（Hamada et al. 1998）、[**Hamada et al. (1999/2000)**](https://arxiv.org/abs/astro-ph/9906419) で詳細論文として公表された。PROGRAPE-1 は、従来のハードワイヤードな GRAPE と異なり、**FPGA**（Field-Programmable Gate Array）を処理要素として用いる。FPGA の論理は再構成可能であるため、**重力に加えて van der Waals 力、SPH の流体力学相互作用など、様々な相互作用**を同一ハードウェアで計算できる。

1998 年の学会発表では、FPGA を採用した設計動機として次の点が述べられている：専用 LSI の代わりに FPGA を用いることで**開発初期費用を下げる**、専用機の利点を保ちつつ**汎用性を得る**。一方で、FPGA は単位ゲート当たり単価が高く大規模システムには不向きである。**ターゲットアプリケーション**として **SPH 法**と**エワルド法**が挙げられており、SPH は計算量が粒子数オーダーのため 10〜100 倍の加速で十分、エワルド法は周期境界条件向けで特定用途に限られる、といった理由で FPGA の柔軟性が活きるとしている。

#### PROGRAPE-1 の仕様と成果

| 項目 | 内容 |
|------|------|
| **ハードウェア** | Altera Flex10K100（EPF10K100）FPGA × 2、座標メモリ |
| **実装パイプライン** | GRAPE-3 と同様の重力相互作用パイプライン、SPH 用プログラミング進行中（1998 年時点） |
| **数値表現** | 減算 20 bit 固定小数点、除算・平方根 14 bit 対数（7 bit 指数・5 bit 仮数）、累積 56 bit 固定小数点 |
| **動作** | 1 チップに 1 パイプライン、16 MHz 動作（Hamada et al. 1999） |
| **性能** | 重力・クーロン力：2.4 Gflops 見積り（1998 年）、0.96 Gflops 実測（1999 年論文）。SPH 法：4.0 Gflops 見積り（1998 年） |

PROGRAPE-1 は、**粒子ベースシミュレーションにおいて重力以外の相互作用（SPH など）の計算コストが高い場合**に特に有用であると結論づけている。

[Kawai et al. (1999)](https://arxiv.org/abs/astro-ph/9909116) の GRAPE-5 論文（Section 6.2）では、**GRAPE-5/PROGRAPE システム**の構想が述べられている。GRAPE-5 が重力の実空間相互作用を計算し、PROGRAPE が SPH の流体力学相互作用や Ewald 法の波数空間成分を計算する。この異種計算機の協調により、GRAPE-5 単体より **少なくとも 10 倍** の高速化が期待されるとされている。

#### PROGRAPE-1 が露呈した課題 → PGPG の動機

PROGRAPE-1 の開発を通じて、**パイプラインの詳細設計、制御論理、ホスト通信、データ変換ライブラリ**の開発に膨大な工数がかかることが明らかになった。PGPG 論文によれば、比較的単純な重力パイプラインの実装でも **1 人年以上**を要した。この設計生産性の壁が、**PGPG**（高レベル記述から VHDL・エミュレータ・インターフェースを一括生成するツール）開発の**直接の動機**となった。

### 1.4 PROGRAPE-3 と SPH パイプライン（2006）

PROGRAPE-1 が 1998 年にターゲットとした **SPH 法**の FPGA 実装は、[**Nakasato et al. (2006)**](https://arxiv.org/abs/astro-ph/0604295) において **PROGRAPE-3** 上で初めて実現された。PROGRAPE-3 は第三世代の PROGRAPE アーキテクチャで、千葉大学・理研共同の **Bioler-3 ボード**（Xilinx XC2VP70-5 FPGA × 4、各 10 万ゲート相当）を用いる。従来の GRAPE では SPH の近傍探索のみを GRAPE で行い、**実際の SPH 相互作用（圧力・粘性・∇・v など）はホスト計算機**で計算されていた。SPH の 1 相互作用あたりの演算量（約 160 flop）は重力（約 38 flop）より大きく、これがボトルネックとなっていた。

Nakasato et al. (2006) では、PGPG の後継である **PGR**（Processors Generators for Reconfigurable systems）を用いて SPH パイプラインを実装した。PGR の重要な拡張は **可変ビット長の浮動小数点演算**のサポートである。PGPG は LNS と整数のみをサポートしていたが、SPH は加減算が多く LNS の利点が少なく、浮動小数点が必須だった。SPH パイプラインの記述は約 200 行程度であり、生成される VHDL は約 7,000 行に及ぶ。

| 項目 | 内容 |
|------|------|
| **ハードウェア** | Bioler-3 ボード、Xilinx XC2VP70-5 × 4、PCI 64bit/66MHz |
| **SPH パイプライン** | 密度・∇・v・∇×v・圧力・人工粘性などを 2 段階で計算 |
| **数値精度** | 仮数部 16 bit、指数 8 bit で Sod 衝撃波管・Cold Collapse を良好に再現 |
| **性能** | SPH ピーク 85 Gflops、現実的な設定でホスト比 **5〜10 倍**の加速 |
| **ボトルネック** | ホスト-PROGRAPE 間のデータ転送が全体時間の約 70% |

Cold Collapse テスト（N=50 万）では、SPH を PROGRAPE、重力をツリー法でホスト計算する構成で約 2.3 倍、SPH・重力ともに PROGRAPE で計算する構成では **約 11 倍**の加速を達成した。1998 年の PROGRAPE-1 発表で掲げた「SPH を FPGA で加速する」という目標が、約 8 年後に PROGRAPE-3 と PGR によって実現された。

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
| Brieu et al. (1995) | GRAPE-3 上で P³M 法を実装（Plummer のみのため 3 回呼び出しで 1 つの PP 力を近似） |
| [Fukushige & Makino (1996)](https://arxiv.org/abs/astro-ph/9612090) | GRAPE-4 による銀河形成 N 体シミュレーション（78 万粒子、332 Gflops、1996 年ゴードン・ベル賞） |
| Makino & Taiji (1998) | GRAPE システムの体系化 |
| [Kawai et al. (1999)](https://arxiv.org/abs/astro-ph/9909116) | GRAPE-5：GRAPE-3 の後継。G5 チップ（2 パイプライン、80 MHz）、38.4 Gflops ピーク、PCI バス、任意 cutoff で Ewald/P³M 対応、対数表現 17(8) bit |

### 2.3 再構成可能計算・PROGRAPE

| 研究 | 内容 |
|------|------|
| Buell et al. (1996) | Splash-1, Splash-2 |
| Hamada et al. (1998) | [PROGRAPE-1 初発表](https://www.asj.or.jp/nenkai/archive/1998a/pdf/X02a.pdf)：日本天文学会春季年会。Flex10K100×2、重力 2.4 Gflops・SPH 4.0 Gflops 見積り、SPH・エワルド法をターゲットに |
| [**Hamada et al. (1999/2000)**](https://arxiv.org/abs/astro-ph/9906419) | **PROGRAPE-1**：FPGA ベース Programmable GRAPE の詳細論文。2× Altera EPF10K100、GRAPE-3 相当パイプライン、0.96 Gflops 実測。重力・van der Waals・SPH への拡張可能性を実証。**PGPG の直接の前身** |
| Spurzem et al. (2002) | FPGA を用いた N 体シミュレーション |
| [**Nakasato et al. (2006)**](https://arxiv.org/abs/astro-ph/0604295) | **PROGRAPE-3 上で SPH パイプラインを初実装**。PGR（PGPG 後継）で浮動小数点をサポートし、SPH を 85 Gflops ピーク、現実設定で 5〜10 倍加速を達成。1998 年 PROGRAPE-1 で掲げた SPH ターゲットを実現 |

### 2.4 高レベル設計手法

当時、デジタル信号処理分野では MATLAB などから HDL を生成するツールが存在した。また、**システムレベル設計**（SLDL）が広まり、SpecC、SystemC、Handel-C などが C/C++ 系でハードウェアを記述する手段として使われ始めていた。しかし、これらは依然として詳細なハードウェア記述を必要とし、PGPG の目指す「パイプラインの高レベル記述から全設計を自動生成する」というアプローチとは異なっていた。

[牧野の「スーパーコンピューティングの将来」](https://jun-makino.sakura.ne.jp/articles/future_sc/note010.html)（2005 年）によれば、FPGA による再構成可能計算は過去 15 年間「未来の技術」のままで、多くのプロジェクトが良い結果を出せていなかった。最大の理由は**設計の困難さ**—シミュレーション専門家と電子工学専門家の共同でも効率の良いハードウェア設計は難しく、1 つの演算精度のパイプラインに 2 年かかると試行錯誤が不可能になる—である。PGPG/PGR はこの「適切なツールがない」問題に対する回答として位置づけられる。

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

PGPG 1.0 は **固定小数点**（fix/ufix）と **対数表現**（log）をサポートする。対数表現では乗除算が加減算になるため、ハードウェアの複雑さと遅延を削減できる。この設計思想は **GRAPE-1** 以来の伝統であり（Ebisuzaki et al. 1993）、GRAPE-3 では 12 bit、[PROGRAPE-1](https://arxiv.org/abs/astro-ph/9906419) では 14 bit（7 bit 指数・5 bit 仮数）の対数表現、[GRAPE-5](https://arxiv.org/abs/astro-ph/9909116) では **17(8) bit**（γ=7 bit 整数部、δ=8 bit 小数部）が用いられた（Kawai et al. 1999）。G5 チップは位置ベクトルに 32 bit 固定小数点、力の累積に 64 bit 固定小数点を採用している。PGPG は GRAPE-5 と同様の設計を採用している。

### 3.5 重力パイプラインの実例

重力加速度 \( f_i = \sum_j m_j \boldsymbol{r}_{ij} / (r_{ij}^2 + \varepsilon^2)^{3/2} \) を PGDL で記述し、GRAPE-3 / GRAPE-5 と同等の設計を自動生成した。Stratix 上では、GRAPE-5 と同様のパイプラインを 5 本、180 MHz で実装可能であり、元の G5 チップ（2 パイプライン、80 MHz、1 クロックあたり 2 ペア相互作用）と比べ約 **5 倍**の性能を達成した。GRAPE-5 ボードは 8 個の G5 チップで 38.4 Gflops ピーク、実効性能は小規模（N=10⁴）でピークの 70% 超を達成している（Kawai et al. 1999）。

### 3.6 今後の拡張計画（論文当時）とその実現

PGPG 論文（2005）が掲げた拡張計画のうち、**SPH パイプライン**と**浮動小数点演算**は [Nakasato et al. (2006)](https://arxiv.org/abs/astro-ph/0604295) で実現された。同論文では PGPG の後継である **PGR** が可変ビット長の浮動小数点をサポートし、PROGRAPE-3 上で SPH パイプライン（密度・∇・v・圧力・人工粘性など）を実装、85 Gflops ピーク・現実設定で 5〜10 倍の加速を達成している。その他の計画：境界要素法（BEM）のサポート、Xilinx FPGA への対応。

---

## 4. その後（2026年現在）の研究開発に与えた影響

### 4.1 高レベル合成（HLS）の普及

PGPG は「高レベル記述からハードウェアを自動生成する」というアプローチを、**天体・粒子シミュレーションというドメイン**に特化して実現した。2010 年代以降、**HLS**（High-Level Synthesis）が FPGA 設計の主流となり、C/C++、SystemC、MATLAB などから RTL を生成する手法が広く普及した。PGPG は、**ドメイン特化型 DSL** によるハードウェア生成の先駆的な例として位置づけられる。

### 4.2 ドメイン特化言語（DSL）による FPGA アクセラレータ設計

2020 年代には、**Spatial**（Stanford）、**Graphitron**、**Scotch** など、**DSL から FPGA アクセラレータを自動生成**する研究が活発になっている。PGPG の「パイプラインの高レベル記述から VHDL、エミュレータ、インターフェースを一括生成する」という設計思想は、これらの DSL と共通する。PGPG は、**ドメイン特化による設計生産性の向上**という方向性を、早期に示した事例として評価できる。

### 4.3 プロジェクトの継続と発展

PGPG の後継として **PGR**（Processors Generators for Reconfigurable systems）が開発され（Hamada & Nakasato 2005）、浮動小数点演算のサポートを追加した。PGR を用いて [Nakasato et al. (2006)](https://arxiv.org/abs/astro-ph/0604295) は **PROGRAPE-3** 上で SPH パイプラインを初実装し、1998 年 PROGRAPE-1 発表時の目標を達成した。PROGRAPE の系譜は PROGRAPE-1 → PROGRAPE-2 → **PROGRAPE-3**（Bioler-3 ボード、4× Xilinx FPGA）と進化している。

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
| **設計** | 共有メモリの制約を考慮したドメイン特化アルゴリズム。j-parallel で部分力を別ブロックで計算するため、GPU 上での reduction が必要でオーバーヘッドが発生 |

#### 42 TFlops 階層的 N 体シミュレーション（SC09 2009）— 多重ウォーク法のブレークスルー

[Hamada et al. (2009)](https://www.cs.umd.edu/class/fall2019/cmsc714/readings/Hamada-nbody.pdf) は、**2009 年ゴードン・ベル賞（価格性能部門）**のエントリーとして、Chamomile Scheme を発展させた **多重ウォーク法（multiple walks method）** により、階層的 N 体法を GPU で初めて高効率に実現した。従来の GPU 実装では、セルあたりの目標粒子数 \(N_g\) が小さいときにパイプライン利用率が低く、階層的アルゴリズムの利点が活かせなかった。

| 先行手法 | 並列化戦略 | 課題 |
|----------|------------|------|
| **i-parallel**（Nyland, Belleman） | 各スレッドが異なる i 粒子の力を計算 | \(N_g\) が数百以下だとプロセッサが遊休 |
| **j-parallel**（Chamomile 等） | j 粒子を分割し、複数ブロックで部分力を計算 | 部分力の GPU 上 reduction が遅く、ホスト通信が \(N_{blocks}\) 倍に増加 |
| **多重ウォーク**（本論文） | **各スレッドブロックを 1 台の GRAPE と見なし**、複数の walk を並列評価 | reduction 不要、ブロックごとに独立した walk を処理 |

多重ウォーク法の流れ：（1）ホスト上で複数 walk の i/j 粒子リストを準備、（2）まとめて GPU へ転送、（3）各ブロックが 1 walk を担当して計算、（4）力を一括でホストへ返却、（5）ホストで軌道積分。Barnes の修正ツリー法と ORB（直交再帰二分法）による負荷分散を併用。GPU では \(N_{crit} \approx 1000\)（CPU は約 32）が最適。**実測では従来 GPU 実装の 2.4 倍、Phantom-GRAPE（最適化 CPU）の 3.2 倍**を達成した。

| 項目 | 重力シミュレーション（ツリーコード） | 乱流シミュレーション（FMM） |
|------|--------------------------------------|-----------------------------|
| **粒子数** | 1,608,044,129 | 16,777,216（256³） |
| **持続性能** | 42.15 TFlops | 20.2 TFlops |
| **補正後性能** | 28.1 TFlops（CPU 最効率アルゴリズム基準の Flops 換算） | — |
| **ハードウェア** | 128 PC × 2 GPU = 256 GPU、総コスト $228,912 | 同上 |
| **価格性能** | 124 MFlops/$ | — |

**FMM on GPU** では、複素球面調和関数を実数基底に変換、翻訳行列をオンデマンド生成、ボックス構造を coalesced 転送に合わせて再編成し、同じ「多重ウォーク」戦略を P2M／M2M／M2L／L2L／L2P 全段に適用。CPU 版と比較して約 **80 倍**の高速化を達成した。乱流計算では **ボルテックス粒子法**（速度は Eq. (1)、渦度の伸長は Eq. (3)、遠距離は FMM）を周期境界の FMM で実装し、スペクトル法との運動エネルギー減衰・エネルギースペクトルの定量的一致を確認している（Yokota et al. 2007）。2006 年 GB ファイナリスト（Kawai et al.）と比較して、粒子数は約 745 倍、補正性能は約 1,826 倍、価格性能は約 19 倍であった。

#### GPU の台頭と FPGA の役割分担

Chamomile Scheme と 42 TFlops 成果の後、**GPU** による N 体シミュレーションが急速に普及した。Sapporo（GRAPE 互換 API を GPU で実装）、CUDA ベースのツリーコード・FMM などが開発され、**2009 年 SC09** で Hamada らは 256 GPU クラスタにより重力 N 体で **42.15 TFlops**（補正後 28.1 TFlops）、価格性能 **124 MFlops/$** を達成してゴードン・ベル賞（価格性能部門）を受賞した。GPU は、**コスト・入手性・プログラミング容易性**の面で GRAPE や FPGA を上回り、天体 N 体シミュレーションの主流となった。

一方、**FPGA** は低電力・高スループットが求められるエッジ AI、データセンターのアクセラレータ、ストリーミング処理などで活躍している。PGPG が示した「**ドメイン特化による設計の簡素化**」というアプローチは、FPGA や ASIC の設計生産性を高める現代的な HLS や DSL の文脈で、再評価される価値がある。また、Chamomile Scheme に代表される GPU アルゴリズム設計も、GRAPE/PROGRAPE で培った「粒子相互作用のパイプライン化」の知見を継承しており、**専用ハードウェア（GRAPE）→ 再構成可能（PROGRAPE/PGPG）→ 汎用並列（GPU）** という技術の連続性が読み取れる。

### 4.5 学術的意義

Ebisuzaki et al. (1993) は、GRAPE を「ユーザーから見ればハードウェアサブルーチン」と表現した。[PROGRAPE-1](https://arxiv.org/abs/astro-ph/9906419) は、その「サブルーチン」を **FPGA 上で再構成可能**にしたが、設計には 1 人年以上を要した。PGPG は、**天体物理学の専門家がハードウェア設計の詳細を学ばずに**、その「サブルーチン」を FPGA 上で自作・カスタマイズできるようにすることを目指した。この「ドメイン専門家の知識」と「ハードウェア設計の詳細」の分離は、**ドメイン特化アーキテクチャ（DSA）** や **DSL の設計**において、今日も重要な設計原則として受け継がれている。

#### 牧野による FPGA・PGR の評価（スーパーコンピューティングの将来 より）

PGPG の共著者である牧野淳一郎は、[「スーパーコンピューティングの将来」](http://jun.artcompsci.org/articles/future_sc.pdf)の [note010](https://jun-makino.sakura.ne.jp/articles/future_sc/note010.html)（2005/12/28）において、**FPGA による再構成可能計算**を論じている。牧野は、FPGA はカスタム LSI に比べて 1/20〜1/1000 の性能にとどまるが、**開発初期コストが不要**となり約 100 万円程度の予算で専用計算機を構築できる利点を指摘する。一方で、再構成可能計算は過去 15 年間「未来の技術」のままで広く普及しておらず、多くのプロジェクトが良い結果を出していないと述べる。

その最大の理由は**設計の困難さ**である。重力パイプラインを FPGA に載せる場合、倍精度浮動小数点をそのまま使うと回路規模は GRAPE-6 の 5 倍、GRAPE-5 の 20 倍以上になり競争力がない。GRAPE はパイプライン部位ごとに演算精度を変え、逆数平方根を表引きと補間多項式で小規模に実現している。**演算精度を最適化した設計**が必須だが、シミュレーション専門家と電子工学専門家の共同では効率の良いハードウェア設計が困難で、1 つの精度で 2 年かかると試行錯誤も不可能になる。

牧野は、**PGR**（濱田・中里による PGPG 後継システム）を「この問題を解決する可能性がある**現在唯一のツール**」と評価している。PGR では演算パイプラインの式を C 風に記述するだけで、ハードウェア記述・通信ライブラリ・エミュレータまで一括生成され、年オーダーの開発期間なしに試せる点が大きな変化であるとしている。FPGA はベクトル計算機の代わりにはならない（メモリバンド幅がボトルネック）が、PC で十分性能が出る計算なら FPGA の検討価値はある、との見解を示している。

### 4.6 研究の系譜（まとめ）

```
GRAPE (1990-)        → 専用ハードウェア、重力のみ
    ↓
PROGRAPE-1 (1998/1999) → FPGA で再構成可能に。1998 年 ASJ 初発表、重力・SPH・van der Waals へ拡張可能
    ↓                 設計生産性の壁（1 人年以上）を露呈
PGPG (2005)          → 高レベル記述から一括生成。設計工数を大幅短縮
    ↓
PGR (2005-)          → 浮動小数点サポートを追加。SPH パイプライン実装に必須
    │
    ├── FPGA 系：PROGRAPE-2 → PROGRAPE-3 (2006)
    │              ↑ Nakasato et al. (2006)：SPH 85 Gflops、5〜10倍加速
    │
    └── GPU 系（並行研究、同一著者）：
        Chamomile Scheme (2007) → CUNBODY-1、256 Gflop/s
            ↓
        多重ウォーク法 → 42 TFlops (SC09 2009)、ゴードン・ベル賞（価格性能部門）
            ↓
        Sapporo、DEGIMA、ツリーコード/FMM on GPU の普及
            ↓
        N 体シミュレーションの主流は GPU へ
```

---

## 5. 低精度計算AI専用チップと開発競争

2020年代、深層学習の爆発的普及に伴い、低精度演算に特化した AI 専用チップの開発競争が全世界で活況を呈している。一見すると本稿の主題とは無縁にも思われるが、実は「ドメイン特化」と「低精度化」による効率追求という点で、GRAPE が 1990 年代に切り拓いた道の延長線上に、現代の AI チップは位置している。当時、日本は**低精度計算専用ハードウェア**の分野で世界を牽引していた。以下、その技術的背景と開発競争の様相を詳述し、技術史の連続性と、日本が置かれた立場の変遷とを論じる。

### 5.1 低精度計算が AI で有効な理由

深層学習や大規模言語モデル（LLM）の推論・学習において、**低精度演算**（8 ビット整数、16 ビット浮動小数点、4 ビット量子化など）が広く採用されている。その根拠は主に次の三つである。

#### 5.1.1 ニューラルネットワークの頑健性

ニューラルネットワークは、**過剰パラメータ化**により冗長な表現を持ち、小さな丸め誤差に対して驚くほど耐性がある。Pete Warden らの研究（2015 年頃）によれば、全結合層・畳み込み層の入力と出力を 8 ビット整数に抑え、**内部の積和演算のみ 32 ビットで累積**すれば、多くのモデルで精度劣化をほぼ観察できない。学習済みモデルは、量子化によって重みと活性化を低ビットに圧縮しても、適切なスケーリングとキャリブレーションを行えば、元の浮動小数点モデルと同等の性能を維持できる。この「低精度入出力＋高精度累積」の構成は、GRAPE が中間演算に対数表現を用いながら力の累積は 56〜64 ビット固定小数点で行っていたのと、設計思想において共通する。

#### 5.1.2 量子化の効果とフォーマット

**量子化**（Quantization）とは、浮動小数点（FP32 や FP16）で表現された重み・活性化を、より狭いビット幅（INT8、INT4、FP8、FP4 など）にマッピングする処理である。アフィン量子化や対称量子化では、スケール因子とゼロ点を決め、元の値域を整数範囲に写像する。これにより、

- **メモリ占有量の削減**（8 ビットで FP32 の 1/4、4 ビットで 1/8）
- **メモリ帯域の節約**（演算ユニットがボトルネックになる前の段階で有効）
- **演算ユニットの小型化・低電力化**（低ビット演算は回路規模・消費電力が小さい）

が得られる。4 ビット量子化では精度劣化のリスクが高く、パー・チャネルスケーリングやキャリブレーションが必須となる。特に LLM では稀なトークンでの誤動作（幻覚、繰り返し）に注意が必要である。主要な低精度フォーマットの例を下表に示す。

| フォーマット | 主な用途 | 特徴 |
|--------------|----------|------|
| **FP32** | 学習・高精度推論 | 従来の標準、高コスト |
| **FP16 / BF16** | 学習・推論 | 半精度、広くサポート |
| **INT8** | 推論 | 量子化の標準、FP32 比で約 4 倍の効率 |
| **FP8** | 推論・学習 | NVIDIA H100、AMD MI300 等でサポート |
| **INT4 / FP4** | 推論（極限圧縮） | 約 8 倍の圧縮、キャリブレーション必須 |
| **FP6** | LLM 推論 | 非 2 のべき乗、柔軟なトレードオフ |
| **NVFP4** | 推論（NVIDIA Blackwell） | 4bit 浮動小数点、FP16 比 3.5 倍メモリ削減、1% 未満の精度劣化 |

NVIDIA の **NVFP4**（2024 年）は、4 ビット浮動小数点により FP16 比で 3.5 倍のメモリ削減と 1.8 倍の FP8 対比削減を実現しつつ、精度劣化を 1% 未満に抑えている。また、FP6 のような非 2 のべき乗フォーマットは、LLM 推論において従来の Tensor Core 対比で単位面積あたり約 1.66 倍の性能向上が報告されるなど、フォーマット最適化の余地が広がっている。

#### 5.1.3 行列積とメモリ律速

深層学習の中核は**行列積**（matrix multiply）である。膨大な MAC（Multiply-Accumulate）演算を行うが、メモリからデータを読む帯域が律速となるケースが多い。低精度化は、1 回のメモリアクセスで更多的な要素を転送でき、実効スループットを向上させる。この「演算律速ではなくメモリ律速」という性質は、GRAPE が粒子データのストリーミングに最適化していたのと、設計上の類似性がある。

### 5.2 低精度計算 AI 専用チップとは

**低精度計算 AI 専用チップ**とは、深層学習の推論・学習に特化し、INT8、FP16、BF16、FP8、INT4 などの低精度演算をハードウェアで高速に実行するプロセッサである。汎用 CPU や従来型 GPU と異なり、行列積・畳み込み・活性化関数に最適化された**Tensor Core（テンサーコア）**や**シストリックアレイ**を備え、電力効率とスループットを最大化する。

#### 5.2.1 代表的な AI 専用チップ

| チップ／システム | 開発元 | 主な低精度サポート | 特徴 |
|------------------|--------|--------------------|------|
| **Google TPU** | Google | v1: INT8、v4: BF16/INT8、最新: FP8 | シストリックアレイ、クラウド専用 |
| **NVIDIA Tensor Core** | NVIDIA | FP64/32/16/8、INT8/4、FP4（Blackwell） | GPU と統合、CUDA エコシステム |
| **AMD Instinct** | AMD | FP8、INT8、FP16、BF16 | MI300/MI325 シリーズ、スパース対応 |
| **Intel Gaudi** | Intel | BF16、FP8、INT8 | 学習・推論両用、オープンソースソフトスタック |
| **AWS Inferentia / Trainium** | Amazon | FP16、BF16、INT8、cFP8 | クラウド推論・学習向け |
| **Meta MTIA** | Meta | 低精度特化（MTIA 450 で強化） | 推論ワークロード向け、チップレット構成 |
| **IBM Spyre** | IBM | FP16、FP8、INT8、INT4 | 混合精度設計、64 低精度エンジン |
| **MN-Core** | Preferred Networks（日本） | 低精度テンソル演算 | Green500 世界 1 位実績、生成 AI 向け L1000 開発中 |

TPU v4 はチップあたり BF16/INT8 で 275 TFLOPS、Google の最新 Ironwood 世代では 9,216 チップのスーパーポッドで FP8 換算 42.5 エクサフロップスを実現する。AWS Inferentia2 は FP16 で 190 TFLOPS、32 GB HBM を備える。AMD Instinct MI325X は FP8/INT8 で 20.9〜41.8 PFLOPs（スパース度による）の生成 AI 性能を掲げる。IBM Spyre は 64 個の低精度エンジンで FP16/FP8/INT8/INT4 の混合精度演算を一から設計している。

#### 5.2.2 アーキテクチャパターン：シストリックアレイと Tensor Core（テンサーコア）

AI 専用チップの代表的なアーキテクチャは次の二つである。

- **シストリックアレイ**：Google TPU が採用。256×256 の INT8 MAC ユニットを格子状に配置し、データを隣接ユニット間でパイプライン的に流す。TPU v1 は 92 TOPS（28〜40 W）を達成した。データの再利用によりメモリアクセスを最小化し、GRAPE のパイプラインが粒子データをストリーミングしたのと同様の「データフロー最適化」の思想を持つ。
- **Tensor Core**：NVIDIA GPU が採用。行列積専用の低精度演算ユニットを GPU 内に統合。FP16、BF16、FP8、INT8、INT4 をハードウェアで直接サポートし、NVIDIA Blackwell では FP4（NVFP4）にも対応。従来の CUDA コア（汎用演算）と併用して、学習・推論の両方でスループットを最大化する。

#### 5.2.3 設計思想：GRAPE との類似

GRAPE は**重力 N 体問題**というドメインに特化し、対数表現（LNS）や固定小数点を用いて演算コストを削減した。低精度 AI 専用チップは**深層学習**というドメインに特化し、行列積と活性化の演算パターンに合わせて低精度ユニットを設計する。どちらも、

1. **ドメイン特化**：汎用演算ではなく、限られた演算パターンに最適化する  
2. **精度と効率のトレードオフ**：必要十分な精度を見極め、それを下回る演算コストを追求する  
3. **並列性の徹底活用**：独立した計算が大量に存在するため、ハードウェア並列度を最大化する  

という点で共通する。PGPG が FPGA 上で「パイプライン設計の高レベル記述から一括生成」により生産性を高めたように、AI チップ開発においても HLS（High-Level Synthesis）やドメイン特化 DSL による設計効率化が重要な課題である。

### 5.3 AI チップ開発競争の様相

2020 年代半ば（2024–2026 年）において、AI チップ市場は激しい競争状態にある。

#### 5.3.1 市場構造

**NVIDIA** はデータセンター向け AI チップ市場で約 **80〜83%** のシェアを占め、デファクトスタンダードたる CUDA と GPU の組み合わせで寡占状態を築いている。2024 年の AI チップ売上は NVIDIA が約 263 億ドルに対し、AMD は約 45 億ドルと格差が大きく、一方で AMD は Microsoft や Meta を顧客に持ち、H200 に対抗する MI325X の出荷を進めている。Intel の Gaudi 3 は特定 LLM で H100 比 50% の高速化を主張し、2024 年後半からサーバービルダー向けに出荷を開始した。顧客の「ベンダーロックイン脱却」願望、調達コストの高騰、各国の産業政策により、**NVIDIA 以外の選択肢**を求める動きが強まっている。また、2024 年には中国で NVIDIA の Mellanox 買収（2020 年）に対する独占禁止法調査が開始されるなど、規制面でも動きがある。

#### 5.3.2 主要プレイヤーと動向

| プレイヤー | 戦略・製品 | 備考 |
|------------|------------|------|
| **NVIDIA** | H100、Blackwell（B100/B200） | FP4（NVFP4）等の新フォーマット、メモリ帯域重視 |
| **AMD** | Instinct MI300/MI325/MI350 | H200 対比で競争力主張、Microsoft・Meta が採用 |
| **Intel** | Gaudi 3、Falcon Shores（計画） | オープンソースソフト、特定 LLM で H100 比 50% 高速化を主張 |
| **Google** | TPU v4、Ironwood | 自社クラウド・研究用途、外部販売は限定的 |
| **Amazon** | Inferentia2、Trainium2 | AWS 専用、推論・学習両対応 |
| **Meta** | MTIA シリーズ | 自社推論負荷向け、低精度サポート強化 |
| **Apple** | Neural Engine（M シリーズ内蔵） | エッジ推論、オンチップ統合 |
| **Qualcomm** | Cloud AI 100、Snapdragon 内蔵 | エッジとクラウドの両面 |
| **中国勢** | 華為海思昇騰、寒武紀、など | 米国制裁下で国内調達・開発を推進 |
| **日本** | Preferred Networks（MN-Core） | スパコン向け実績、生成 AI 向け L1000 開発（2026 年目標） |

#### 5.3.3 オープンソース基盤の動き

NVIDIA の CUDA への依存を弱めるため、**Intel、Google、Arm、Qualcomm** らが **UXL（Unified Acceleration Foundation）** を設立し、オープンソースのアクセラレーション基盤を推進している。ハードウェア多様化に伴い、ソフトウェアのポータビリティとベンダー非依存が重要な競争軸となっている。

#### 5.3.4 各国の産業政策

米国、欧州、中国、台湾、韓国、日本などが、AI チップの国内生産・開発を産業戦略の中心に据えている。**米国**は CHIPS 法に基づき半導体製造・研究開発に巨額の補助金を投入し、TSMC、Samsung、Intel 等の国内拠点拡大を支援している。**欧州**は Chips for Europe を掲げ、研究・製造・設計の強化を図る。**中国**は米国制裁下で華為海思昇騰、寒武紀など国産 AI チップの開発を急ピッチで推進し、調達制限への対応を迫られている。**台湾**は TSMC を擁し、先進プロセスによる AI チップ製造の中心地としての地位を維持している。**韓国**は Samsung 等による HBM 等のメモリ技術と、AI チップ設計の連携を強化している。**日本**は半導体・デジタル産業施策として Rapidus 等の製造拠点支援や、Preferred Networks への投資促進（後述）に取り組んでいる。補助金、半導体製造支援、人材育成、学術研究との連携など、政策的な後押しが開発競争を加速させている。

### 5.4 GRAPE 時代の日本と現代の対比

1990 年代、**GRAPE** は天体 N 体シミュレーションにおいて、低精度（対数表現・固定小数点）による専用ハードウェアで、汎用スーパーコンピュータの **100〜10,000 倍** の価格性能比を実現した。GRAPE-4 は 1996 年ゴードン・ベル賞、GRAPE-6 は 2003 年同特別賞を受賞している。PGPG や PROGRAPE は、その設計思想を継承しつつ、FPGA による再構成可能性と高レベル設計ツールで設計生産性を高めた。当時の日本は、**低精度計算専用ハードウェア**の分野で世界を牽引する存在だった。

2020 年代の AI チップ開発競争において、日本は **Preferred Networks（PFN）** の MN-Core シリーズが一定の存在感を示している。第 1 世代 MN-Core は 2020〜2021 年に Green500 で世界 1 位を 3 度獲得し、第 2 世代 MN-Core 2 は 2023 年稼働開始、2024 年 9 月から販売を開始した。2024 年 8 月には SBI ホールディングスと次世代 AI 半導体開発に向けた資本業務提携（最大 100 億円規模の出資予定）を締結、同年 11 月には生成 AI 推論専用プロセッサ **MN-Core L1000** の開発を発表している。L1000 は三次元積層 DRAM 技術を採用し、既存 GPU と比較して最大 10 倍の高速処理を目指し、2026 年の提供を目標としている。一方で、NVIDIA、AMD、Google、Amazon などに比べて市場規模・エコシステム・顧客基盤の面で存在感は限定的であり、技術の継承、産業構造の変容、国際競争環境の変貌、人材・資金の流れなど、多角的な考察が今後求められる。本稿で扱う PGPG や GRAPE の系譜は、日本が低精度計算専用ハードウェアの分野で築いた**知の遺産**であり、その設計思想は現代の AI チップ開発にも通じる。次代を担う研究者・技術者が、この遺産を足がかりに新たな挑戦を始めることへの期待が、結語に込められている。

---

## 6. 参考文献

### 論文本体

- Hamada, T., Fukushige, T., & Makino, J. (2005). PGPG: An Automatic Generator of Pipeline Design for Programmable GRAPE Systems. *Publications of the Astronomical Society of Japan*, 57(5), 799–813. [DOI](https://doi.org/10.1093/pasj/57.5.799)
- arXiv: [astro-ph/0703182](https://arxiv.org/abs/astro-ph/0703182)

### 主要参考文献

- Barnes, J., & Hut, P. (1986). Barnes-Hut ツリー法
- Sugimoto, D., et al. (1990). GRAPE プロジェクト開始
- [Ebisuzaki, T., Makino, J., Fukushige, T., Taiji, M., Sugimoto, D., Ito, T., & Okumura, S. K. (1993). GRAPE Project: An Overview. *PASJ*, 45, 269–278](https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1993PASJ...45..269E&defaultprint=YES&filetype=.pdf)
- [**Kawai, A., Fukushige, T., Makino, J., & Taiji, M. (1999). GRAPE-5: A Special-Purpose Computer for N-body Simulation. arXiv:astro-ph/9909116**](https://arxiv.org/abs/astro-ph/9909116) — GRAPE-3 の後継。G5 チップ、38.4 Gflops ピーク、PCI バス、任意 cutoff で Ewald/P³M 対応
- [Fukushige, T., & Makino, J. (1996). N-body Simulation of Galaxy Formation on GRAPE-4 Special-Purpose Computer. arXiv:astro-ph/9612090](https://arxiv.org/abs/astro-ph/9612090)
- Makino, J., & Taiji, M. (1998). GRAPE システム
- Hamada, T., Fukushige, T., Kawai, A., & Makino, J. (1998). PROGRAPE-1: プログラム可能な超高速多体シミュレーション専用計算機. *日本天文学会 1998 年春季年会* X02a. [PDF](https://www.asj.or.jp/nenkai/archive/1998a/pdf/X02a.pdf) — **PROGRAPE-1 の初発表**
- [**Hamada, T., Fukushige, T., Kawai, A., & Makino, J. (1999/2000). PROGRAPE-1: A Programmable, Multi-Purpose Computer for Many-Body Simulations. *PASJ*, 52, 943. arXiv:astro-ph/9906419](https://arxiv.org/abs/astro-ph/9906419) — **PGPG の直接の前身。FPGA による Programmable GRAPE の詳細論文**
- [**Hamada, T., & Iitaka, T. (2007). The Chamomile Scheme: An Optimized Algorithm for N-body simulations on Programmable Graphics Processing Units. arXiv:astro-ph/0703100](https://arxiv.org/abs/astro-ph/0703100) — **PGPG と同時期・同一著者。GPU 向け N 体シミュレーション、CUNBODY-1**
- [**Hamada, T., Narumi, T., Yokota, R., Yasuoka, K., Nitadori, K., & Taiji, M. (2009). 42 TFlops Hierarchical N-body Simulations on GPUs with Applications in both Astrophysics and Turbulence. SC09.**](https://www.cs.umd.edu/class/fall2019/cmsc714/readings/Hamada-nbody.pdf) — **多重ウォーク法による階層的 N 体法（ツリーコード・FMM）の GPU 高効率実装。重力 16 億粒子で 42.15 TFlops、乱流 1600 万粒子で 20.2 TFlops。2009 年ゴードン・ベル賞（価格性能部門）、124 MFlops/$**
- [**Nakasato, N., Hamada, T., & Fukushige, T. (2006). SPH Simulations with Reconfigurable Hardware Accelerator. arXiv:astro-ph/0604295**](https://arxiv.org/abs/astro-ph/0604295) — **PROGRAPE-3 上で SPH パイプラインを初実装。PGR で浮動小数点をサポート、85 Gflops ピーク、5〜10 倍加速**
- 牧野淳一郎. スーパーコンピューティングの将来. [PDF](http://jun.artcompsci.org/articles/future_sc.pdf)、[HTML 版（目次）](https://jun-makino.sakura.ne.jp/articles/future_sc/face.html) — **note010「FPGA と再構成可能計算」**で PGPG/PGR を「設計問題を解決する可能性がある現在唯一のツール」と評価

### 関連リソース

- [PROGRAPE-1 初発表（日本天文学会 1998 年春季年会 X02a）](https://www.asj.or.jp/nenkai/archive/1998a/pdf/X02a.pdf)
- [SPH Simulations with Reconfigurable Hardware Accelerator（Nakasato et al. 2006）](https://arxiv.org/abs/astro-ph/0604295)
- [スーパーコンピューティングの将来（牧野淳一郎）](http://jun.artcompsci.org/articles/future_sc.pdf) — FPGA・PGR に関する論考（[note010](https://jun-makino.sakura.ne.jp/articles/future_sc/note010.html) 等）
- [PROGRAPE 公式サイト](http://progrape.jp)
- [CUNBODY-1（Chamomile Scheme 実装）](https://github.com/thamada/cunbody1)
- [42 TFlops Hierarchical N-body Simulations on GPUs（Hamada et al. 2009, SC09）](https://www.cs.umd.edu/class/fall2019/cmsc714/readings/Hamada-nbody.pdf) — 2009 年ゴードン・ベル賞（価格性能部門）
- [本リポジトリ README](../README.md)
- [設計仕様書](../doc/design.md)

---

## 7. 結語

2026年現在、AIブームに伴う低精度計算AI専用チップの開発競争が全世界で活況を呈している。この状況を1990年代の低精度天文計算専用チップ（GRAPE 等）の開発と照らし合わせると、当時の日本は同分野において世界を牽引する先進的な技術力を有していたことが窺われる。他方、現在の低精度計算AIチップ開発競争において日本が十分な存在感を示し得ていない現状は、技術の継承、産業構造の変容、および国際競争環境の変貌といった観点から、今後の考察が待たれるところである。本稿が扱うPGPGやGRAPEの系譜は、日本が低精度計算専用ハードウェアの分野で築いた知の遺産であり、次代を担う若い研究者・技術者たちにとって、単なる過去の記録ではなく新たな挑戦の足がかりとなり得る。先人たちが「不可能」を「可能」に変えてきたように、今度は若い世代の手で次の時代を切り拓く創造が始まることを、心より期待する。

最終更新: 2025年3月14日
