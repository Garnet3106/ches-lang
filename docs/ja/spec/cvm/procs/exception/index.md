# 処理手順 - 例外

[ドキュメント](../../../../../index.md) > [日本語](../../../../index.md) > [仕様書](../../../index.md) > [CVM](../../index.md) > [処理手順](../index.md) > [例外](./index.md)

## 概要

## 送出

演算スタックから例外参照値を取得し、ポップする。

関数スタック領域に例外参照値を格納する。

## 検知, 捕捉

関数スタック領域から例外参照値を取得し、値が `0xFFFFFFFF` である場合は例外処理をスキップする。

有効な例外参照が格納されている場合は、値を `0xFFFFFFFF` に置き換えた後 `ret` 命令を実行する。

---

Copyright © 2019-2021 Garnet3106 All rights reserved.
