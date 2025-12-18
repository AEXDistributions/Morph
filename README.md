# Morph - Media Processing Engine

**Latest:** 0.0.36 (Alpha Release)  
**Author:** [wzssmex@gmail.com](mailto:wzssmex@gmail.com)

Morph is designed as a **foundation for building powerful image processing workflows**. Whether you're automating batch operations, building graphics applications, or integrating image manipulation into your development pipeline, Morph provides the tools you need. This version introduces optimized in-memory processing, automatic folder structure creation, a new preview system, and enhanced workflow flexibility.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Status](https://img.shields.io/badge/Status-In%20Development-orange)](https://github.com/AEXFoundation/morph)

---

## Overview

### Purpose

Morph is designed as a **foundation for building powerful image processing workflows**. Whether you're automating batch operations, building graphics applications, or integrating image manipulation into your development pipeline, Morph provides the tools you need.

### Core Features

* **Multi-Language Support:** Native support for Python, C, and C++. Use it directly in scripts, embed it in applications, or integrate it into your UI frameworks.
* **Extensible Architecture:** Built to support a comprehensive library of composable image filters. Chain and combine filters to create sophisticated processing pipelines.
* **Command-Line Interface:** Direct command-line access makes it ideal for automation, scripting, and cross-platform workflows.
* **Optimized Processing:** Efficient memory management for fast batch operations and filter chaining.
* **Non-Destructive Workflow:** Original files remain untouched, preview before final export.

---

## Folder Structure

On first run, Morph automatically creates:
```
Morph/
├── input/    (reserved for future use)
└── output/   (default preview and export destination)
```

---

## Usage Guide

### 1. Input & Output System (`-i` / `-o`)

This system uses the `@` symbol to clearly designate file paths. All commands and filter names are **case-insensitive**, but file paths preserve their original casing.

#### A. Input (`-i @<path>`)

| Command | Purpose | Rules & Notes |
| :--- | :--- | :--- |
| **`-i @"C:\path\to\file.png"`** | Loads a single image file for processing. | Path must be enclosed in quotes if it contains spaces. |
| **`-i @"C:\path\to\folder"`** | Loads all supported images from the folder. | Supported file types: `.png`, `.jpg`, `.jpeg`, `.bmp`, `.tga`, `.gif`, `.webp`, `.tif`, `.tiff`. Filenames are stored in **lowercase**. |

**Example:**
```bash
> -i @"C:\Photos\vacation"
Loading: img001.jpg
Loading: img002.jpg
...
Loaded 50 image(s)
```

#### B. Output (`-o @<path> [mode] [filename]`)

The path (`@`), mode (`keep`/`clear`), and optional filename can appear in any order after `-o`.

| Argument | Description | Default |
| :--- | :--- | :--- |
| **`@<path>`** | The required destination folder for the export. | (None - Must be supplied) |
| **`[mode]`** | Controls the action after export: **`clear`** (remove from pipeline) or **`keep`** (retain for further processing). | `clear` |
| **`[filename]`** | Optional. Exports only this specific file. If omitted, all files are exported. | All files |

**Examples:**

* **Export All and Clear Pipeline (Default):**
  ```bash
  > -o @"C:\Output\Folder"
  ```

* **Export All but Keep in Pipeline:**
  ```bash
  > -o keep @"C:\Output\Folder"
  ```

* **Export Single File and Clear it:**
  ```bash
  > -o @"C:\Output" clear image.jpg
  ```

* **Flexible Syntax:**
  ```bash
  > -o keep image.jpg @"C:\Output"
  ```

### 2. Preview System (`preview`)

Save processed images to the default `Morph/output` folder for quick review without committing to a final export location.

| Command | Purpose | Rules & Notes |
| :--- | :--- | :--- |
| **`preview`** | Saves all processed images to `Morph/output` folder. | Images remain in the pipeline for further processing. |
| **`preview <filename>`** | Saves only the specified image to `Morph/output`. | Useful for checking individual results. |

**Example:**
```bash
> preview
Saving preview to: Morph/output
[OK] img001.jpg
[OK] img002.jpg
Saved 50 preview(s) to disk
```

### 3. Pipeline Management & Information (`@i`)

| Command | Purpose | Rules & Notes |
| :--- | :--- | :--- |
| **`@i`** | **Show Images:** Lists all current images in the processing pipeline with details. | Shows image dimensions and processing status. |
| **`help`** | Shows all available commands. | Displays command reference. |
| **`exit`** | Exits the program. | Case-insensitive (`EXIT`, `exit`, `Exit` all work). |

**Example:**
```bash
> @i
Images in pipeline (50):
  - img001.jpg (4032x3024, 34.86 MB)
  - img002.jpg (4032x3024, 34.86 MB) [MODIFIED]
  ...
```

### 4. Filter Application (`@i <filter> <percent> [filename]`)

Filters are applied to the images currently in the processing pipeline. Processing is optimized for speed, allowing rapid iteration and experimentation.

| Command | Purpose | Rules & Notes |
| :--- | :--- | :--- |
| **`@i grayscale 100%`** | Applies full Grayscale filter to **all** images. | The filter name is case-insensitive. |
| **`@i grayscale 50%`** | Applies 50% Grayscale blend to **all** images. | Percentage controls the intensity of the effect. |
| **`@i grayscale 75% image.jpg`** | Applies 75% Grayscale filter to the single file `image.jpg`. | The filename must match the lowercase name in the pipeline. |

**Grayscale Filter Details:**
- Uses weighted RGB conversion: `0.299R + 0.587G + 0.114B`
- Percentage (0-100%) controls blend with original colors
- 100% = full grayscale, 0% = no effect

**Example:**
```bash
> @i grayscale 60%
Applying grayscale (60%)...
[OK] img001.jpg
[OK] img002.jpg
...
```

---

## Typical Workflow

### Basic Processing
```bash
> -i @"C:\Photos\vacation"         # Load images
> @i                                # View loaded images
> @i grayscale 50%                  # Apply filter
> preview                           # Quick preview in Morph/output
> -o @"C:\Final\Processed"          # Export to final location
```

### Experimenting with Filter Strength
```bash
> -i @"photo.jpg"                   # Load one image
> @i grayscale 25%                  # Try 25% strength
> preview                           # Check result
> @i grayscale 50%                  # Try 50% strength
> preview                           # Check again
> -o @"C:\Best"                     # Export when satisfied
```

### Processing Specific Images
```bash
> -i @"C:\Mixed"                    # Load folder
> @i grayscale 100% pic1.jpg        # Full grayscale on pic1
> @i grayscale 50% pic2.jpg         # Partial grayscale on pic2
> -o keep @"C:\Out" pic1.jpg        # Export pic1, keep working
> -o @"C:\Out" pic2.jpg             # Export pic2, clear pipeline
```

### Batch Processing with Preview
```bash
> -i @"C:\RawPhotos"                # Load all photos
> @i grayscale 40%                  # Apply consistent filter
> preview                           # Preview in Morph/output
> -o @"C:\Instagram\Ready"          # Export finals
```

---

## Command Reference

### Input Commands
- `-i @"path"` - Load image(s) from file or folder

### Processing Commands
- `@i` - List all images in pipeline with details
- `@i grayscale <percent>` - Apply grayscale to all images
- `@i grayscale <percent> <filename>` - Apply grayscale to specific image

### Output Commands
- `preview` - Save all to Morph/output (keep in pipeline)
- `preview <filename>` - Save specific image to Morph/output
- `-o @"path"` - Export all and clear pipeline
- `-o keep @"path"` - Export all but keep in pipeline
- `-o @"path" <filename>` - Export specific image

### Utility Commands
- `help` - Show command help
- `exit` / `quit` - Exit program

---

## Performance Features

### Optimized Processing
- Images are processed efficiently with minimal overhead
- Filter operations are optimized for speed
- Support for chaining multiple filters without performance degradation
- Fast preview generation for quick iteration

### Memory Management
- Efficient handling of large image batches
- Automatic cleanup when exporting
- Pipeline status shows processing state and image details

### Batch Operations
- Process hundreds of images quickly
- Consistent filter application across entire batches
- Flexible export options for different workflows

---

## Technical Specifications

### Supported Formats
**Input:** `.png`, `.jpg`, `.jpeg`, `.bmp`, `.tga`, `.gif`, `.webp`, `.tif`, `.tiff`  
**Output:** `.png`, `.jpg`, `.bmp` (format preserved from original)

### Filter Details
- **Grayscale**: Weighted RGB conversion (ITU-R BT.601 standard)
- **Blend Mode**: Percentage-based mixing with original colors
- **Precision**: 8-bit per channel processing

### System Requirements
- C++17 or later
- Standard filesystem support
- STB image libraries (included)

---

## Building

```bash
# Compile with C++17 support
g++ -std=c++17 main.cpp -o morph -lstdc++fs

# Run the program
./morph
```

---

## Use Cases

### Photography Workflows
- Batch process vacation photos
- Create consistent looks across image sets
- Quick preview before committing to exports
- Non-destructive editing workflow

### Graphics Applications
- Integrate into larger processing pipelines
- Automate repetitive image operations
- Build custom filter chains
- Process assets for games or applications

### Automation & Scripting
- Command-line batch processing
- Integration with shell scripts
- Automated image preparation
- CI/CD pipeline integration

---

## Architecture Highlights

### Non-Destructive Processing
- Original files are never modified
- All processing happens on loaded copies
- Preview and export are separate steps
- Safe experimentation without data loss

### Flexible Pipeline
- Load images once, apply multiple filters
- Preview at any stage
- Export to multiple locations
- Keep or clear images as needed

### Extensible Design
- Easy to add new filters
- Composable filter architecture
- Support for filter chaining
- Clean separation of concerns

---

## Future Enhancements

- Additional filters (blur, sharpen, brightness, contrast)
- Batch export with different formats
- Filter presets and macros
- Python and C bindings
- GUI integration support

---

## License

MIT License - See LICENSE file for details

---

[© 2025 Aether, All Rights Reserved](https://www.instagram.com/aetherstudi0s/)
