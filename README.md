# Morph

A console-based image processing engine that uses nodes to apply and combine filters, enabling complex image transformations directly from the command line.

## Features

- **Node-Based Architecture**: Chain filters together to create complex processing pipelines
- **Command-Line Interface**: Seamlessly integrate into any workflow or automation script
- **Extensible Filter Library**: Designed to support a vast collection of composable image filters
- **Universal Integration**: Works with any application or development environment

## Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/morph.git
cd morph

# Install dependencies
npm install

# Build
npm run build
```

## Quick Start

```bash
# Basic filter application
morph input.png --filter blur --output output.png

# Chain multiple filters
morph input.png --filter blur --filter sharpen --filter contrast --output output.png

# Using node syntax
morph input.png --node "blur(5) -> sharpen(2) -> output" --output result.png
```

## Usage

Morph processes images through a node-based pipeline where each node represents a filter or operation.

### Basic Syntax

```bash
morph <input> [options] --output <output>
```

### Available Filters

- `blur` - Gaussian blur effect
- `sharpen` - Sharpening filter
- `contrast` - Adjust image contrast
- `brightness` - Adjust image brightness
- `grayscale` - Convert to grayscale
- `invert` - Invert colors
- *(More filters coming soon)*

## Examples

```bash
# Apply single filter
morph photo.jpg --filter grayscale --output bw_photo.jpg

# Complex pipeline
morph image.png --filter blur(3) --filter contrast(1.5) --filter sharpen --output enhanced.png

# Batch processing
for file in *.jpg; do
  morph "$file" --filter vintage --output "processed_$file"
done
```

## Development

### Building Filters

Filters follow a standard interface:

```javascript
export function filterName(image, params) {
  // Process image data
  return processedImage;
}
```

### Contributing

Contributions are welcome! Please read our [CONTRIBUTING.md](CONTRIBUTING.md) for details on how to submit filters, report issues, and contribute to the project.

## Roadmap

- [ ] Core filter library (50+ filters)
- [ ] Advanced node composition syntax
- [ ] Performance optimization for batch processing
- [ ] Plugin system for third-party filters
- [ ] GUI node editor (optional companion tool)
- [ ] Video frame processing support

## License

MIT License - see [LICENSE](LICENSE) for details

## Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/morph/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/morph/discussions)
- **Documentation**: [Wiki](https://github.com/yourusername/morph/wiki)

---

Built for developers who need powerful, scriptable image processing in their workflow.
