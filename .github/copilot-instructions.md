# Novot リポジトリ向け Copilot 指示

## プロジェクト概要

Novot は Windows 向けの C++20 製 2D ゲームエンジンライブラリです。
Visual Studio の静的ライブラリとしてビルドされ、DxLib がウィンドウ、
入力、2D 描画、テクスチャ、音声、タイマーを担当します。Effekseer による
2D エフェクトにも対応しています。生成物は主に `Novot.lib` で、利用側の
ゲームプロジェクトからリンクして使用します。

主な実行経路は `_Novot::StartGame()` が DxLib/Effekseer を初期化して
`Setup()` を呼び、フレームごとに `_Novot::Update()` と `_Novot::Draw()` を
実行する構造です。更新では入力、タイマー、一時停止、フレーム単位実行、
時間倍率、シーン、Effekseer を処理し、描画ではバックバッファを消去して
シーンツリーとエフェクトを描画します。

`SceneManager` は現在のシーンとエフェクトシーンを管理します。`Scene` は
階層構造を持つ基本オブジェクトで、子シーンは親の位置、拡大率、可視性、
アルファ、色調の影響を受けます。派生クラスは `_Load`、`_Update`、
`_Draw`、`_Release` を実装して振る舞いを追加します。`Object2D` は速度、
加速度、移動、衝突処理を追加し、`Sprite2D`、`Button`、Grid コンテナなどが
再利用可能なシーン部品として用意されています。

主要サブシステムは `Core/` 以下にあります。`Asset/` は参照数を管理する
アセット、テクスチャ、スプライトシート、シェーダー、サウンドを扱い、
`Collision/` はポリゴン衝突、マウス衝突、レイキャスト、地形衝突を扱います。
`Math/` はベクトルや幾何計算、`Animation/` は Tween とイージング、
`Input/`、`Mouse/`、`Event/`、`Timer`、`Camera` は共通サービスです。
`Debugger/` は Debug ビルド専用の Win32 デバッグ UI です。

共通サービスの多くは `Singleton<T>` パターンを使い、`SceneManager`、
`AssetManager`、`GameState`、`KeyMapping`、`PropertyManager` などのマクロで
アクセスします。既存サブシステムを拡張するときは、この方式を維持します。

## ビルド

リポジトリルートの Developer PowerShell または Developer Command Prompt で
次を実行します。

```powershell
msbuild .\Novot.vcxproj /m /p:Configuration=Debug /p:Platform=x64
msbuild .\Novot.vcxproj /m /p:Configuration=Release /p:Platform=x64
```

プロジェクトは C++20、v145 ツールセット、Debug/Release、Win32/x64 構成を
使用します。DxLib と EffekseerForDXLib のローカルインストールが必要で、
インクルードパスとライブラリパスは現在 `Novot.vcxproj` に設定されています。
`Novot.slnx` は兄弟ディレクトリ `../Houkou` も参照するため、Novot だけを
確認する場合や兄弟プロジェクトがない場合は `Novot.vcxproj` を直接ビルドします。

テストプロジェクト、テストランナー、Lint 設定はありません。単一テストを
実行するコマンドもありません。利用側ゲームの実行ファイルがある場合は、
それを使ってランタイム動作を確認します。`.vs/` と `x64/` は生成物です。

## 実装上の規約

- ランタイムオブジェクトは `Scene` から派生し、ライフサイクルフックで実装します。
  子シーンの追加には `CreateScene<T>()` または `AddScene()` を使います。
- シーン所有のリソースは `Scene::LoadAsset<T>()` で取得します。
  `AssetManager` がシーン ID で参照を管理するため、共有アセットを個別に解放しません。
- `position`、`offset`、衝突頂点はローカル座標です。親変換やカメラを考慮する場合は
  `GetGlobalPosition()`、`GetScale()`、`GetDrawPosition()` を使います。
- 子シーンは更新時に逆順、描画時に正順で処理されます。この順序を変更しません。
- 座標や色には既存の `Vector2D`、`Vector3D`、`NovotRenderer` を再利用します。
- 操作は `KeyMapping::RegisterMapping()` で登録し、`IsActionPressing()` で取得します。
- 一時停止、フレーム単位実行、時間倍率は `GameState` を使います。
- 既存の `Signal` がある通知は、独自ポーリングではなくシグナルを使います。
- Debugger API とプロパティバインディングは `_DEBUG` ガードを維持します。
- アセットは `Assets/` からの相対パスで指定し、キャッシュキーにも同じパスを使います。
- 新しいソースは `Novot.vcxproj` と `Novot.vcxproj.filters` の両方に登録します。
- エンジンヘッダーは `<Scene.h>` や `<Math/Vector2D.h>` の形式を優先します。
- `.vs/`、`x64/`、生成データベース、オブジェクト、Copilot インデックスは編集しません。
