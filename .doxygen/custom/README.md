# Personalización de Doxygen - Estilo Texas Instruments

Este directorio contiene los archivos de personalización para la documentación generada con Doxygen.

## Estructura del proyecto

```
.doxygen/
├── Doxyfile           # Configuración principal de Doxygen
└── custom/
    ├── custom.css     # Estilos personalizados (tema TI)
    └── README.md      # Esta documentación
```

## Archivos incluidos

### `custom.css`
Hoja de estilos personalizada con el tema de Texas Instruments:
- **Colores principales**: Rojo TI (#CC0000), gris, blanco
- **Diseño profesional** similar a la documentación de TI
- **Responsive** para dispositivos móviles
- **Tablas mejoradas** con encabezados en rojo
- **Bloques de código** con borde rojo distintivo

## Cómo personalizar

### 1. Cambiar colores
Edita las variables CSS en `custom.css`:

```css
:root {
    --ti-red: #CC0000;           /* Color principal */
    --ti-dark-red: #990000;      /* Rojo oscuro */
    --ti-gray: #666666;          /* Texto secundario */
    --ti-light-gray: #F5F5F5;    /* Fondos */
}
```

### 2. Agregar tu logo

1. Guarda tu logo en este directorio (ej: `logo.png`)
2. Edita el `Doxyfile` en `.doxygen/`:
   ```
   PROJECT_LOGO = custom/logo.png
   ```
3. Dimensiones recomendadas: **200x55 píxeles** (máximo)

### 3. Personalizar encabezado/pie de página

Desde el directorio `.doxygen/`, genera los archivos base:
```bash
cd .doxygen
doxygen -w html custom/header.html custom/footer.html custom/stylesheet.css
```

Luego edítalos y configura en `Doxyfile`:
```
HTML_HEADER = custom/header.html
HTML_FOOTER = custom/footer.html
```

## Configuración actual en Doxyfile

Las siguientes opciones están configuradas:

```
HTML_EXTRA_STYLESHEET = custom/custom.css
HTML_COLORSTYLE = LIGHT
GENERATE_TREEVIEW = YES
HTML_DYNAMIC_MENUS = YES
PROJECT_NAME = "DevLab_ICM20948"
PROJECT_BRIEF = "Driver para sensor ICM-20948"
OUTPUT_DIRECTORY = ../docs
INPUT = ../src ../examples
```

## Ejemplos de personalización

### Cambiar a tema azul
```css
:root {
    --ti-red: #0066CC;
    --ti-dark-red: #004499;
    --ti-light-red: #3399FF;
}
```

### Cambiar a tema verde
```css
:root {
    --ti-red: #00AA00;
    --ti-dark-red: #008800;
    --ti-light-red: #00CC00;
}
```

## Referencias

- [Doxygen Documentation](https://www.doxygen.nl/manual/)
- [Texas Instruments Style Guide](https://www.ti.com)
- [CSS Variables](https://developer.mozilla.org/en-US/docs/Web/CSS/Using_CSS_custom_properties)

## Notas

- Los cambios en CSS se aplicarán automáticamente al regenerar la documentación
- Para probar localmente: `cd .doxygen && doxygen Doxyfile` y abre `docs/html/index.html`
- El workflow de GitHub Actions aplica estos estilos automáticamente
