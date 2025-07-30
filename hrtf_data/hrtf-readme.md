# HRTF Data Directory

This directory should contain the MIT KEMAR HRTF database files.

## Download Instructions

1. Visit the MIT KEMAR HRTF Database:
   - URL: http://sound.media.mit.edu/resources/KEMAR.html

2. Download the "full" dataset (contains all elevations)

3. Extract the contents and place the `elev0` folder here:
   ```
   hrtf_data/
   └── MIT_KEMAR/
       └── elev0/
           ├── L0e000a.wav
           ├── L0e030a.wav
           ├── L0e120a.wav
           ├── L0e240a.wav
           ├── L0e330a.wav
           ├── R0e000a.wav
           ├── R0e030a.wav
           ├── R0e120a.wav
           ├── R0e240a.wav
           ├── R0e330a.wav
           └── ... (other angle files)
   ```

## Resampling to 48kHz

The MIT KEMAR files are originally at 44.1kHz. You need to resample them to 48kHz:

```bash
cd MIT_KEMAR/elev0
for f in *.wav; do
    sox "$f" -r 48000 "48k_$f"
done

# Then rename the resampled files
for f in 48k_*.wav; do
    mv "$f" "${f#48k_}"
done
```

## Required Files

For this project, we specifically need these HRTF files at 0° elevation:
- **L0e000a.wav** / **R0e000a.wav** - Center (0°)
- **L0e030a.wav** / **R0e030a.wav** - Front Right (30°)
- **L0e120a.wav** / **R0e120a.wav** - Rear Right (120°)
- **L0e240a.wav** / **R0e240a.wav** - Rear Left (240°)
- **L0e330a.wav** / **R0e330a.wav** - Front Left (330°)