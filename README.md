# 42_scop
So Corporate OpenGL Program

## Usage
```sh
make
./scop objects/Suzanne.obj textures/uv_grid.tga
```

## Example
https://user-images.githubusercontent.com/53996617/158661797-f9dec216-8dd1-416e-adb0-11956c32400f.mp4

### Normals display
![Screenshot_20220317_062659](https://user-images.githubusercontent.com/53996617/158743144-e8779497-b26b-416d-9a5c-0e5833e1ddf1.png)

## Shortcuts
### General keymaps
| Key | Description |
| --- | --- |
| `ESC` | Exit the program |
| `M` | Cycle between GL_FILL, GL_LINE, and GL_POINT polygon mode |
| `R` | Toggle model rotation |
| `O` | Toggle perspective/orthographic projection |
| `N` | Toggle normals display |
| `L` | Toggle lighting |
| `T` | Toggle texture on model (smooth transition) |
| `C` | Cycle between model colors (texture has to be off) |
| `WheelUp` | Increase free flight mode speed |
| `WheelDown` | Decrease free flight mode speed |
| `Ctrl` - `WheelUp` | Zoom In by decreasing FOV |
| `Ctrl` - `WheelDown` | Zoom Out by decreasing FOV |
### Free flight keymaps
Free flight mode is enabled when `RMB` is hold.

Moving the mouse simulate a first person view.
| Key | Description |
| --- | --- |
| `W` | Move forward |
| `S` | Move backward |
| `A` | Strafe left |
| `D` | Strafe right |
| `Q` | Move down |
| `E` | Move up |
| `Shift` | Double movements speed by holding |
| `Alt` | Divide by 2 movements speed by holding |
### Model movements keymaps
| Key | Description |
| --- | --- |
| `X` | Select X axis |
| `Y` | Select Y axis |
| `Z` | Select Z axis |
| `-` | Move the model in the negative direction of the currently selected axis |
| `=` | Move the model in the positive direction of the currently selected axis |
| `Shift` | Double movements speed by holding |
| `Alt` | Divide by 2 movements speed by holding |
