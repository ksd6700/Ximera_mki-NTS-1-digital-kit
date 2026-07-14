# Ximera_mki-NTS-1-digital-kit

Ximera MKI は、Nord Lead 系の明るい VA リード、JP-8080 系の広がる supersaw、Minimoog 系の太い sub と drive を「足して三で割った」ような Nu:Tekt NTS-1 digital kit mkI 向けユーザー oscillator です。

1 oscillator の中で 7 本の band-limited saw、square/parabolic のエッジ、sub oscillator、内部 tone filter、soft drive をまとめています。NTS-1 単体で太い mono lead、広がる chord stab、少し丸い bass を作るための小さな音源です。

## Download

- Latest built unit: https://github.com/ksd6700/Ximera_mki-NTS-1-digital-kit/releases/latest
- Source repository: https://github.com/ksd6700/Ximera_mki-NTS-1-digital-kit
- Official logue SDK: https://github.com/korginc/logue-sdk
- KORG NTS-1 Librarian / updater downloads: https://www.korg.com/us/support/download/product/0/832/

Release assets に `ximera_mki.ntkdigunit` がある場合は、それを KORG NTS-1 Librarian で NTS-1 mkI に転送してください。

## Controls

| Control | Meaning |
| --- | --- |
| Shape | Warm/tight から bright/wide へ進む macro |
| Shift + Shape | Edge: square/parabolic の噛みつきと微妙な位相差 |
| DETUNE | 7 saw voice の detune 幅 |
| SPREAD | detuned voice の存在感と phase spread |
| SUB | 1 octave 下の square/sine sub |
| DRIVE | soft clip 前段の押し込み |
| TONE | 内部 2-pole tone filter の明るさ |
| MODE | `0`: lead, `1`: wide, `2`: bass |

NTS-1 本体の cutoff / resonance も内部 tone filter に軽く反映します。外部 filter と合わせると、リードでは少し眩しく、ベースでは少し丸く追い込めます。

## Build

This repository expects KORG's official logue SDK to live outside the project.

```sh
git clone https://github.com/korginc/logue-sdk.git
cd logue-sdk
git submodule update --init --recursive
cd tools/gcc
./get_gcc_5_4-2016q3_macos.sh
```

Then build Ximera MKI:

```sh
cd /path/to/Ximera_mki-NTS-1-digital-kit
make package LOGUE_SDK_PATH=/path/to/logue-sdk
```

The package will be written to:

```text
ximera_mki.ntkdigunit
```

## Compatibility

- Target: Nu:Tekt NTS-1 digital kit mkI
- logue SDK API: `1.1-0`
- Unit format: `.ntkdigunit`

The NTS-1 mkI shares the logue SDK v1 oscillator format family with prologue and minilogue xd, but this project is tuned and named for NTS-1 digital kit mkI.

## License

Original Ximera MKI oscillator code is released under the MIT License.

`tpl/_unit.c` is derived from KORG logue SDK template code and keeps KORG's BSD 3-Clause notice. The SDK itself is not vendored here; download it from the official KORG repository above.
