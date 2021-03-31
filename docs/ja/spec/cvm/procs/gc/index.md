# 処理手順 - GC

[ドキュメント](../../../../../index.md) > [日本語](../../../../index.md) > [仕様書](../../../index.md) > [CVM](../../index.md) > [処理手順](../index.md) > [GC](./index.md)

## 概要

## 新規データ領域のコレクション

参照スタック内の以下に当てはまる要素を削除対象として検索する。

- `0xFFFFFFFF` から始まる要素
- 削除基準の GC 回数が設定 `gc-rem-count` の数値を超える場合

削除対象でない要素を、新規データ領域内の使用されていなかった片方の領域にデータを詰めてコピーする。

コピー時に削除基準の GC 回数を対象に 1 を加算する。

## 長期データ領域のコレクション

参照スタック内の以下に当てはまる要素を削除対象として検索する。

- `0xFFFFFFFF` から始まる要素

---

Copyright © 2019-2021 Garnet3106 All rights reserved.