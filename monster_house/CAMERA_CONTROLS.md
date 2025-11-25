# Modificación del Control de Cámara con Scroll del Ratón

## Cambios Implementados

### 1. **InputController - Nueva funcionalidad de pitch**

Se agregó una nueva variable `cameraPitch` que controla el ángulo vertical de la cámara:

```cpp
float cameraPitch;  // Ángulo vertical de la cámara (inicializado en 0.0f)
```

### 2. **Método `processScroll()` modificado**

El comportamiento del scroll ahora **depende de la cámara activa**:

#### **Primera Persona (activeCamera = true)**
El scroll del ratón controla el pitch (ángulo vertical) de la cámara:

```cpp
if (activeCamera) {
    // Primera persona: ajustar el pitch de la cámara
    cameraPitch += (float)yoffset * 2.0f; // Sensibilidad del scroll
    
    // Limitar el pitch entre -50 y +50 grados
    if (cameraPitch > 50.0f)
        cameraPitch = 50.0f;
    if (cameraPitch < -50.0f)
        cameraPitch = -50.0f;
    
    updateCameraDirection();
}
```

**Características:**
- Sensibilidad: 2.0 grados por unidad de scroll
- Límite superior: +50 grados (mirando hacia arriba)
- Límite inferior: -50 grados (mirando hacia abajo)

#### **Tercera Persona (activeCamera = false)**
El scroll del ratón controla la distancia de la cámara (zoom):

```cpp
else {
    // Tercera persona: ajustar distancia de la cámara
    trdpersonOffset -= (float)yoffset * 0.5f;
    if (trdpersonOffset < 1.0f) 
        trdpersonOffset = 1.0f;
    if (trdpersonOffset > 15.0f) 
        trdpersonOffset = 15.0f;
}
```

**Características:**
- Comportamiento tradicional de cámara tercera persona
- Distancia mínima: 1.0 unidades
- Distancia máxima: 15.0 unidades

### 3. **Método `updateCameraDirection()` actualizado**

Calcula la dirección de la cámara de forma diferente según la perspectiva:

```cpp
void updateCameraDirection() {
    glm::vec3 front;
    float yawRad = glm::radians(rotateCharacter);
    float pitchRad = glm::radians(cameraPitch);
    
    if (activeCamera) {
        // Primera persona: aplicar pitch completo
        front.x = cos(pitchRad) * sin(yawRad);
        front.y = sin(pitchRad);
        front.z = cos(pitchRad) * cos(yawRad);
        camera.Front = glm::normalize(front);
    } else {
        // Tercera persona: solo rotación horizontal (sin pitch)
        front.x = sin(yawRad);
        front.y = 0.0f; // Sin inclinación vertical
        front.z = cos(yawRad);
        camera3rd.Front = glm::normalize(front);
    }
    
    // forwardView permanece horizontal para ambas cámaras
    forwardView.x = sin(yawRad);
    forwardView.y = 0.0f;
    forwardView.z = cos(yawRad);
    forwardView = glm::normalize(forwardView);
}
```

**Importante:** 
- **Primera Persona**: La cámara puede mirar arriba/abajo libremente
- **Tercera Persona**: La cámara mantiene una vista horizontal, sin inclinación vertical
- `forwardView` se mantiene en el plano horizontal en ambos casos para que el personaje se mueva correctamente

### 4. **Función `Update()` simplificada**

Se eliminó la actualización manual del `camera.Front` ya que ahora se maneja automáticamente:

```cpp
// Update cameras
if (activeCamera) {
    // Primera persona
    camera.Position = position;
    camera.Position.y += 1.4f;
    // El Front ya está actualizado por updateCameraDirection()
} else {
    // Tercera persona
    camera3rd.Position = position;
    camera3rd.Position.y += 1.5f;
    camera3rd.Position -= trdpersonOffset * forwardView;
    // El Front ya está actualizado por updateCameraDirection()
}
```

## Controles del Usuario

### **En Primera Persona (Cámara 1)**
- **Scroll Arriba**: Mira hacia arriba (máximo +50°)
- **Scroll Abajo**: Mira hacia abajo (máximo -50°)
- **Mouse Horizontal**: Rota al personaje (horizontal)

### **En Tercera Persona (Cámara 3)**
- **Scroll Arriba**: Acerca la cámara al personaje
- **Scroll Abajo**: Aleja la cámara del personaje
- **Mouse Horizontal**: Rota al personaje (horizontal)
- La cámara **NO** puede inclinarse verticalmente

### Movimiento del Personaje (Ambas Cámaras)
- **W/?**: Avanzar
- **S/?**: Retroceder
- **A/?**: Girar izquierda
- **D/?**: Girar derecha

### Otros Controles
- **C**: Cambiar entre 1ª y 3ª persona
- **L**: Mostrar/ocultar indicadores de luz
- **M/N/B**: Modos de debug (wireframe/fill/point)

## Comportamiento Técnico

1. **Separación de comportamientos por cámara:**
   - **Primera Persona**: Control completo de pitch vertical con scroll
   - **Tercera Persona**: Control de distancia/zoom con scroll (comportamiento clásico)

2. **Prevención de pitch en tercera persona:**
   - La cámara de tercera persona siempre mantiene una vista horizontal
   - Esto evita ángulos extraños que podrían desorientar al jugador

3. **Límites de pitch (solo primera persona):**
   - +50°: Evita que la cámara se voltee completamente
   - -50°: Evita que la cámara se voltee completamente

4. **Transición suave entre cámaras:**
   - Al cambiar de cámara con 'C', el pitch se preserva
   - En tercera persona, el pitch simplemente no se aplica a la vista

## Ventajas

? **Primera Persona**: Control intuitivo para mirar arriba/abajo
? **Tercera Persona**: Control de distancia tradicional (como en juegos AAA)
? Mantiene el movimiento del personaje en el suelo
? Límites de rotación para evitar desorientación (solo 1ª persona)
? Comportamiento específico y apropiado para cada tipo de cámara
? Transición fluida entre perspectivas

## Comparación de Funcionalidades

| Característica | Primera Persona | Tercera Persona |
|---------------|----------------|-----------------|
| **Scroll Arriba** | Mira +50° arriba | Acerca cámara |
| **Scroll Abajo** | Mira -50° abajo | Aleja cámara |
| **Pitch Vertical** | ? Sí | ? No |
| **Zoom/Distancia** | ? No | ? Sí |
| **Rotación Horizontal** | ? Sí | ? Sí |
