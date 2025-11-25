# Light Indicators Feature

## Descripción
Se han agregado indicadores visuales (esferas pequeñas) que muestran la posición de las luces en la escena.

## Controles
- Presiona la tecla **L** para mostrar/ocultar los indicadores de luz
- Los indicadores se muestran como esferas brillantes en las posiciones exactas de las fuentes de luz

## Características Técnicas
- **Variable Global**: `showLightIndicators` (bool) - controla la visibilidad de los indicadores
- **Clase**: `LightIndicator` - maneja la renderización de las esferas en las posiciones de las luces
- **Shaders**: 
  - `viaje_lunar/shaders/light_indicator.vs` - vertex shader
  - `viaje_lunar/shaders/light_indicator.fs` - fragment shader

## Posiciones de las Luces
1. Luz 1: (500, 200, 500)
2. Luz 2: (-500, 200, 500)
3. Luz 3: (500, 200, -500)
4. Luz 4: (-500, 200, -500)

## Implementación
- Los indicadores son esferas de baja resolución (10 sectores x 10 stacks) para optimizar el rendimiento
- El color de cada indicador coincide con el color de su luz correspondiente
- Los indicadores se renderizan después de todos los objetos de la escena
- El tamaño de las esferas es de 5 unidades de mundo

## Uso
```cpp
// Toggle the indicators
showLightIndicators = !showLightIndicators;  // or press L key

// Add a new light indicator
lightIndicator->addLight(position, color);
```
