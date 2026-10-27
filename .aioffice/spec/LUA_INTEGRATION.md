# Luaシーン機能

## 合意した仕様

- Lua 5.5.1のソースをNovotリポジトリに含め、静的ライブラリへ組み込む。
- Luaでテーブルを返し、シーンツリーを構成する。
- C++で作成したシーンへLuaスクリプトを添付できる。
- Lua状態はシーン単位で所有する。
- 組み込みシーン型は`Scene`、`Object2D`、`Sprite2D`、`AnimatedSprite2D`、`TextLabel`、`ColorRect`、`Button`。

## 実装済みAPI

- `LuaSceneLoader::Load(path, errorMessage)`でLua定義からシーンを構築する。
- `Scene::AttachLuaScript(path)`で既存シーンへスクリプトを添付する。
- `LuaSceneLoader::RegisterType`でゲーム固有の`Scene`派生型を生成し、Luaノードのプロパティを設定できる。
- 共通プロパティとして`name`、`position`、`offset`、`scale`、`alpha`、`visible`、`enabled`を公開する。
- Luaコールバックとして`on_load`、`on_update`、`on_release`を呼び出す。
- Buttonの押下・解放、AnimatedSprite2Dのアニメーション変更・終了イベントに対応する。

ゲーム固有の型登録はプロセス内のLuaロード時設定です。登録型のConfiguratorにLuaノードテーブルを渡し、型固有プロパティの解釈をゲーム側へ委ねます。組み込み7型は上書きできません。Map1固有のカメラ、マップデータ、遷移、音響、シェーダー、エディター保存形式はNovot共通仕様に含めず、ゲーム側の実装対象です。既存JSONマップとDebugエディターはそのまま利用できます。

使用例と制限は[Luaシーンの利用](../../docs/LUA_SCENES.md)を参照。
