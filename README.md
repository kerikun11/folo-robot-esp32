# FOLO Robot Retrofitting

エレキットのロボット「フォロ」の改造版。ESP32マイコンで制御。

## 開発環境

- [PlatformIO - Espressif32](https://docs.platformio.org/en/latest/platforms/espressif32.html)
  - VSCode: 拡張機能 `PlatformIO` をインストール
  - コマンドライン開発: `pip install platformio`

### 実行例

```sh
# firmware をビルドする (VSCode: Ctrl+Alt+B)
platformio run
# firmware を書き込む (VSCode: Ctrl+Alt+U)
platformio run -t upload
# filesystem image を書き込む (VSCode: PlatformIO Tab → Upload FileSystem Image)
platformio run -t uploadfs
```
