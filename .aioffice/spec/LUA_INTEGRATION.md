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
- 共通プロパティとして`name`、`position`、`offset`、`scale`、`alpha`、`visible`、`enabled`を公開する。
- Luaコールバックとして`on_load`、`on_update`、`on_release`を呼び出す。
- Buttonの押下・解放、AnimatedSprite2Dのアニメーション変更・終了イベントに対応する。

使用例と制限は[Luaシーンの利用](../../docs/LUA_SCENES.md)を参照。
