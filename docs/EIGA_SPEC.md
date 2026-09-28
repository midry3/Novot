# Eigaスクリプト
# キャラクター名登録
`.eigadef`に以下の形式で、スクリプトで使用する名前と話者として表示する名前の対応を作ることが出来る。
```
character: キャラクター名
```

# `@`: 話者切り替え
```
@character
```
で話者の切り替えが可能

# `#` 指示
```
#setpicl character
#chpic character ファイル名(拡張子抜き)
```
など。
`@` の前に書く

# シグナル
```
[Signal]
```
で`Signal`を発火することが出来る。ランタイム側で`Signals.AddSignal("Signal")`をすると、シグナルを発行し受け付けることが出来る。
