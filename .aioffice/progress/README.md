# 進捗

作業の状況、完了したこと、次に行うこと、阻害要因を記録します。詳細な日次ログなどは別ファイルに分けても構いません。

## Luaシーン機能

- Lua 5.5.1のソースを同梱し、Visual Studio静的ライブラリへ追加。
- `LuaSceneLoader::Load`でLuaテーブルからシーンツリーを生成。
- `Scene::AttachLuaScript`でC++シーンにLuaスクリプトを添付。状態は各シーンが所有。
- `on_load`、`on_update`、`on_release`、ButtonとAnimatedSprite2Dのイベント関数に対応。
- 使用例: [Luaシーンの利用](../../docs/LUA_SCENES.md)。確定仕様: [Luaシーン機能](../spec/LUA_INTEGRATION.md)。
- 検証: Debug x64で`msbuild .\Novot.vcxproj /m:1 /p:Configuration=Debug /p:Platform=x64`成功。
- 制限: ゲーム固有の型登録、スクリプトからのシーンツリー変更、Debuggerへのエラー統合は未対応。
