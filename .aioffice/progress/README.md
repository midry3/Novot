# 進捗

作業の状況、完了したこと、次に行うこと、阻害要因を記録します。詳細な日次ログなどは別ファイルに分けても構いません。

## Luaシーン機能

- Lua 5.5.1のソースを同梱し、Visual Studio静的ライブラリへ追加。
- `LuaSceneLoader::Load`でLuaテーブルからシーンツリーを生成。
- `Scene::AttachLuaScript`でC++シーンにLuaスクリプトを添付。状態は各シーンが所有。
- `on_load`、`on_update`、`on_release`、ButtonとAnimatedSprite2Dのイベント関数に対応。
- `LuaSceneLoader::RegisterType` / `UnregisterType`を追加し、ゲーム固有の`Scene`派生型をLuaツリーへ登録可能にした。ConfiguratorからLuaノードテーブルと定義ファイルのパスを受け取れる。
- 使用例: [Luaシーンの利用](../../docs/LUA_SCENES.md)。確定仕様: [Luaシーン機能](../spec/LUA_INTEGRATION.md)。
- Map1移行案はNovot共通機能として型拡張口を実装。JSON読込とDebugエディターの保存形式は維持し、Map1固有の定義変換・音響・遷移・シェーダー制御はEnvSoundEnc側の作業として残る。
- 検証: Debug x64で`msbuild .\Novot.vcxproj /m:1 /p:Configuration=Debug /p:Platform=x64 /p:DebugInformationFormat=OldStyle /p:MultiProcessorCompilation=false`成功。
- 制限: スクリプトからのシーンツリー変更、Debuggerへのエラー統合、ゲーム固有イベント購読は未対応。
