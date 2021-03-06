# ChesC コマンド

[ドキュメント](../../../../index.md) > [日本語](../../../index.md) > [仕様書](../../index.md) > [コマンドライン](../index.md) > [ChesC コマンド](./index.md)

## 概要

`ChesC` はChesコンパイラを使用するコマンドである。

## ファイルパス

```
{$HOME_DIR}/bin/chesc0
```

## サブコマンド

### cmp

ChesコードをCVMコードにコンパイルする。

#### 引数と機能

|オプション名|オプション引数|機能|備考|
|:-:|:-:|:-:|:-:|
|-i|入力ファイルパス|入力元を指定する||
|-limit|`-1` または `0` 以上の数値|ログの最大表示数を指定する|`-1` ... 無制限|
|-o|出力ファイルパス|出力先を指定する||

#### 代替コマンド

サブコマンド名 `cmp` を省略可。

例: `$ chesc cmp -i Main.ches` → `$ chesc -i Main.ches`

### help

ヘルプを表示します。

#### 引数と機能

|オプション名|オプション引数|機能|
|:-:|:-:|:-:|
|なし|-|helpコマンドのヘルプを表示する|
|-cmd|コマンド名|コマンドヘルプを表示する|

---

Copyright © 2019-2021 Garnet3106 All rights reserved.
