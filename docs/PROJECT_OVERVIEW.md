# Novot プロジェクト概要

Novot は、DxLib を基盤にした Windows 向け C++20 製の 2D ゲームエンジン
ライブラリです。Visual Studio の静的ライブラリとしてビルドされ、ゲーム側
プロジェクトから `Novot.lib` をリンクして利用する構成です。Effekseer による
2D エフェクトにも対応しています。

## 実行の流れ

`Novot.cpp` の `_Novot::StartGame()` が DxLib と Effekseer を初期化し、
ゲーム側の `Setup()` を呼び出してフレームループを開始します。

- `Update()` はデバッガーコマンド、入力、タイマー、ゲーム状態、シーン、
  Effekseer を更新します。
- `Draw()` は画面を消去し、現在のシーンとエフェクトを描画します。
- `GameState` により一時停止、フレーム単位の実行、時間倍率を制御できます。

## シーン構造

`SceneManager` が現在のシーンを保持し、`Scene` がゲームオブジェクトの基本
クラスになります。シーンは子シーンを持つツリー構造で、親の位置、拡大率、
可視性、アルファ、色調が子に反映されます。

ゲーム固有のオブジェクトは `Scene` から派生し、`_Load`、`_Update`、
`_Draw`、`_Release` を実装します。`Object2D` は移動、速度、加速度、衝突を
追加した 2D オブジェクトです。`Sprite2D`、`Button`、Grid コンテナなどは
標準の再利用可能なシーン部品です。

## 主な機能

- **アセット**: `AssetManager` がテクスチャ、スプライトシート、サウンド、
  シェーダーなどをキャッシュし、シーン ID ごとの参照数で解放します。
- **描画**: `NovotRenderer` が線、ポリゴン、矩形、点を描画し、DxLib の
  2D 描画を共通化します。
- **衝突**: ポリゴン衝突、衝突レイヤー、マウス衝突、レイキャスト、地形衝突を
  提供します。衝突イベントは `Signal` で通知します。
- **アニメーション**: `Tween`、`TweenGroup`、イージング、タイマーを
  シーンから生成できます。
- **入力**: `KeyMapping` にアクション名とキーを登録し、ゲームコードから
  生のキーコードを意識せず入力を確認できます。
- **デバッグ**: Debug ビルドでは Win32 UI にログ、フレーム時間、CPU/メモリ、
  シーンツリー、プロパティ、時間制御、アセット状態を表示できます。

## ビルド環境

必要な外部ライブラリは DxLib と EffekseerForDXLib です。パスは
`Novot.vcxproj` に設定されています。Developer PowerShell などで次を実行します。

```powershell
msbuild .\Novot.vcxproj /m /p:Configuration=Debug /p:Platform=x64
```

現在、テストプロジェクトや Lint 設定はありません。`Novot.slnx` は
`../Houkou` の兄弟プロジェクトも参照するため、Novot 単体の確認では
`Novot.vcxproj` を直接ビルドします。
