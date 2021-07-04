# 配列

[ドキュメント](../../../../../index.md) > [日本語](../../../../index.md) > [仕様書](../../../index.md) > [言語仕様書](../../index.md) > [データ構造](../index.md) > [配列](./index.md)

## 概要

同じ型の値が並んだ固定長の集合である。

要素数はコンパイル時に決定される必要がある。

## 構文

```
#== 配列型変数の定義 ==#

let arr = Arr<str>[3s32, 1s32, 0s32, 6s32]

# >> "32"
println(arr.get(2))

# 範囲外を参照するためエラー
println(arr.get(4))
```

## インデックス

`0` から始まり、負数値 `n` を指定するとインデックス `length + n` を指すことになる。

---

Copyright © 2019-2021 Garnet3106 All rights reserved.