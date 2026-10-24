# Luaシーンの利用

Lua 5.5.1はNovotに同梱され、Novot.libへ静的に組み込まれます。ゲーム側では`Core/Lua/LuaSceneLoader.h`をインクルードして利用します。

## Luaテーブルからシーンを読み込む

```lua
local function on_start_pressed(button, event)
    print("start button pressed")
end

return {
    type = "Scene",
    name = "Title",
    children = {
        {
            type = "TextLabel",
            name = "TitleText",
            text = "Novot",
            position = { x = 640, y = 120 },
            width = 800
        },
        {
            type = "Button",
            name = "StartButton",
            position = { x = 640, y = 500 },
            size = { x = 220, y = 80 },
            events = {
                left_pressed = "on_start_pressed"
            }
        }
    }
}
```

```cpp
std::string error;
auto scene = LuaSceneLoader::Load("scenes/title.lua", &error);
if (scene) {
    SceneManager.SetCurrentScene(scene);
}
else {
    DebuggerWriteLine(error);
}
```

対応する型は`Scene`、`Object2D`、`Sprite2D`、`AnimatedSprite2D`、`TextLabel`、`ColorRect`、`Button`です。共通プロパティは`name`、`position`、`offset`、`scale`、`alpha`、`visible`です。`Sprite2D`系は`texture`、`TextLabel`は`text`と`width`、`ColorRect`は`size`と`color`を設定できます。テクスチャの相対パスはLuaファイルの場所から解決します。

イベント名はButtonで`left_pressed`、`left_released`、AnimatedSprite2Dで`animation_changed`、`animation_finished`を使えます。Buttonに`area`の頂点配列または`size`を指定するとクリック領域になります。イベント関数には対象シーンとイベント情報が渡ります。マウスイベント情報は`position`、`global_position`、`just`を持つテーブルです。

## C++シーンにスクリプトを付ける

```cpp
class GameScene : public Scene {
protected:
    void _Load() override {
        auto button = CreateScene<Button>();
        button->SetName("StartButton");
        button->SetArea(Vector2D(0, 0), Vector2D(220, 0), Vector2D(220, 80), Vector2D(0, 80));
        button->AttachLuaScript("scripts/start_button.lua");
    }
};
```

```lua
function on_load(scene)
    scene.position = { x = 100, y = 80 }
end

function on_update(scene, dt)
    scene.position = {
        x = scene.position.x + 10 * dt,
        y = scene.position.y
    }
end

function on_left_pressed(button, event)
    print(button.name, event.position.x, event.position.y)
end
```

スクリプト状態は各`Scene`が個別に所有します。`on_load`、`on_update`、`on_release`は省略できます。Buttonへ添付した場合は`on_left_pressed`と`on_left_released`が接続され、AnimatedSprite2Dへ添付した場合は`on_animation_changed`と`on_animation_finished`が接続されます。シーンの解放時に接続を切り、Lua状態を閉じます。

## Luaから操作できるSceneプロパティ

`scene.name`、`scene.alpha`、`scene.visible`、`scene.enabled`は読み書きできます。`scene.position`、`scene.offset`、`scene.scale`は`{ x = ..., y = ... }`のテーブルです。

## 制限

- Lua定義のノード型は組み込みの7型です。ゲーム固有のC++派生型をLua定義へ登録するAPIは今後の拡張項目です。
- Luaシーンファイルは構築時とコールバック実行時にそれぞれ読み込まれるため、ファイルのトップレベルでは副作用のある処理を避けてください。
- Lua標準ライブラリを全て開いています。信頼できないLuaファイルを実行する用途には適しません。
- Lua C APIのエラーは標準エラーへ出力されます。
