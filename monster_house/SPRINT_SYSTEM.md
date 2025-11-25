# Sistema de Sprint (Correr) - Documentación

## ?? Nueva Funcionalidad: Correr con Shift

Se ha implementado un sistema de sprint que permite al personaje moverse más rápido al mantener presionada la tecla Shift junto con las teclas de movimiento.

---

## ?? Controles

### **Movimiento Normal**
```
W / ?  ? Caminar hacia adelante
S / ?  ? Caminar hacia atrás
```
**Velocidad:** 1.0x (normal)

### **Sprint (Correr)**
```
Shift + W / ?  ? CORRER hacia adelante
Shift + S / ?  ? CORRER hacia atrás
```
**Velocidad:** 2.5x (2.5 veces más rápido)

---

## ?? Implementación Técnica

### **1. Nueva Variable `runMultiplier`**

```cpp
float runMultiplier;  // Multiplicador de velocidad para correr
```

**Valor por defecto:** `2.5f`
- Significa que al correr te mueves 2.5 veces más rápido
- Configurable con `setRunMultiplier(float multiplier)`

### **2. Detección de Shift**

```cpp
bool isRunning = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || 
                  glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
```

**Características:**
- ? Detecta **Shift Izquierdo** y **Shift Derecho**
- ? Ambos funcionan de la misma manera
- ? Compatibilidad con diferentes teclados

### **3. Cálculo de Velocidad Dinámica**

```cpp
float currentSpeed = isRunning ? scaleV * runMultiplier : scaleV;

// Aplicar velocidad al movimiento
if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    position = position + currentSpeed * forwardView;
}
```

**Funcionamiento:**
- **Sin Shift:** `currentSpeed = 0.01 * 1.0 = 0.01` (velocidad normal)
- **Con Shift:** `currentSpeed = 0.01 * 2.5 = 0.025` (velocidad de sprint)

### **4. Métodos de Configuración**

```cpp
void setRunMultiplier(float multiplier) { runMultiplier = multiplier; }
float getRunMultiplier() const { return runMultiplier; }
```

**Uso:**
```cpp
inputController->setRunMultiplier(3.0f);  // Correr 3x más rápido
inputController->setRunMultiplier(1.5f);  // Correr 1.5x más rápido
```

---

## ?? Comparación de Velocidades

| Modo | Multiplicador | Velocidad Real | Descripción |
|------|---------------|----------------|-------------|
| **Caminar** | 1.0x | 0.010 u/frame | Velocidad normal |
| **Correr** | 2.5x | 0.025 u/frame | Sprint activo |

*u/frame = unidades por frame*

---

## ?? Casos de Uso

### **Exploración Detallada**
```
Control: W (sin Shift)
Uso: Examinar áreas pequeñas, inspeccionar objetos
Velocidad: Lenta y precisa
```

### **Navegación Rápida**
```
Control: Shift + W
Uso: Recorrer grandes distancias, exploración general
Velocidad: Rápida y eficiente
```

### **Combinaciones Avanzadas**

#### **Sprint en Diagonal**
```
Shift + W + A/D (rotando)
? Correr mientras giras
? Movimiento fluido y rápido
```

#### **Sprint con Cámaras Diferentes**

**Primera Persona + Sprint:**
```
Shift + W (corriendo)
Scroll Arriba/Abajo (mirando)
? Exploración rápida con control de vista
```

**Tercera Persona + Sprint:**
```
Shift + W (corriendo)
Scroll Arriba/Abajo (zoom)
? Vista panorámica mientras corres
```

---

## ?? Ajustes y Personalización

### **Cambiar Velocidad de Sprint**

En `Start()` después de crear el `inputController`:

```cpp
// Ejemplo 1: Sprint más rápido (3x)
inputController->setRunMultiplier(3.0f);

// Ejemplo 2: Sprint moderado (2x)
inputController->setRunMultiplier(2.0f);

// Ejemplo 3: Sprint extremo (5x)
inputController->setRunMultiplier(5.0f);
```

### **Cambiar Velocidad Base**

Modificar en el constructor de `InputController`:

```cpp
InputController(...) 
    : ... scaleV(0.015f), ...  // Caminar más rápido (era 0.01f)
```

### **Tabla de Configuraciones Recomendadas**

| Escenario | scaleV | runMultiplier | Resultado |
|-----------|--------|---------------|-----------|
| **Realista** | 0.008 | 2.0 | Movimiento natural |
| **Equilibrado** | 0.010 | 2.5 | **Actual (por defecto)** |
| **Arcade** | 0.015 | 3.0 | Rápido y divertido |
| **Exploración** | 0.005 | 2.0 | Preciso pero con opción rápida |

---

## ?? Características Destacadas

### ? **Transición Instantánea**
- No hay aceleración/desaceleración
- Al presionar Shift: inmediatamente más rápido
- Al soltar Shift: inmediatamente velocidad normal

### ? **Compatible con Todas las Direcciones**
```
Shift + W  ? Sprint adelante   ?
Shift + S  ? Sprint atrás      ?
```

### ? **Independiente de la Cámara**
- Funciona igual en primera persona
- Funciona igual en tercera persona
- No afecta la rotación ni el pitch

### ? **Sin Conflictos**
- No interfiere con otros controles
- Shift + teclas normales siguen funcionando
- No afecta al cambio de cámara (C)
- No afecta a los indicadores de luz (L)

---

## ?? Guía de Uso para el Jugador

### **Inicio del Juego**
1. Usa **W/S** para familiarizarte con la velocidad normal
2. Prueba **Shift + W** para experimentar el sprint
3. Alterna entre ambos modos según la situación

### **Durante la Exploración**

#### **Escenario 1: Área Abierta (Superficie Lunar)**
```
Recomendación: Shift + W
Razón: Grandes distancias, sprint es eficiente
```

#### **Escenario 2: Interior de la Casa**
```
Recomendación: W (sin Shift)
Razón: Espacio reducido, control preciso
```

#### **Escenario 3: Observando la Luna**
```
Acción:
1. Caminar sin prisa (W)
2. Primera persona (C)
3. Mirar arriba (Scroll)
4. Disfrutar de la vista ??
```

---

## ?? Solución de Problemas

### **Problema: "No puedo correr"**
```
Solución:
- Verifica que estés presionando Shift + W simultáneamente
- Prueba con el Shift derecho si el izquierdo no funciona
- Asegúrate de mantener Shift presionado mientras te mueves
```

### **Problema: "Sprint muy lento"**
```
Solución:
- El multiplicador actual es 2.5x
- Puedes aumentarlo modificando el código
- Ver sección "Ajustes y Personalización"
```

### **Problema: "Sprint muy rápido"**
```
Solución:
- Reducir runMultiplier a 2.0x o 1.5x
- O reducir scaleV base
```

---

## ?? Métricas de Rendimiento

### **Comparación de Tiempos**

Distancia a recorrer: **100 unidades**

| Modo | Tiempo (frames) | Tiempo (seg @60fps) |
|------|-----------------|---------------------|
| Caminar | ~10,000 frames | ~167 segundos |
| Correr | ~4,000 frames | ~67 segundos |
| **Ahorro** | **60%** | **100 segundos** |

---

## ?? Mejores Prácticas

### **Para el Jugador:**
1. ?? Usa sprint para trayectos largos
2. ?? Camina en áreas con objetos cercanos
3. ?? Alterna según la situación
4. ?? Combina con las cámaras para mejor control

### **Para el Desarrollador:**
```cpp
// ? BUENO: Configuración equilibrada
scaleV = 0.01f;
runMultiplier = 2.5f;

// ? EVITAR: Demasiado rápido
scaleV = 0.1f;
runMultiplier = 10.0f;  // Incontrolable

// ? BUENO: Personalización por nivel
if (nivel == "interior") {
    inputController->setRunMultiplier(1.5f);  // Menos velocidad
} else if (nivel == "exterior") {
    inputController->setRunMultiplier(3.0f);   // Más velocidad
}
```

---

## ?? Mejoras Futuras (Opcionales)

### **1. Sistema de Stamina**
```cpp
float stamina = 100.0f;
if (isRunning && stamina > 0) {
    stamina -= deltaTime * 10.0f;  // Consumir stamina
    currentSpeed = scaleV * runMultiplier;
} else {
    currentSpeed = scaleV;  // Caminar si no hay stamina
}
```

### **2. Sonidos de Pasos**
```cpp
if (isRunning) {
    playSound("footsteps_run.wav", 1.5f);  // Pasos rápidos
} else {
    playSound("footsteps_walk.wav", 1.0f); // Pasos normales
}
```

### **3. Animaciones**
```cpp
if (isRunning) {
    character->playAnimation("run");
} else {
    character->playAnimation("walk");
}
```

### **4. Efectos Visuales**
```cpp
if (isRunning) {
    camera.addShake(0.01f);  // Vibración de cámara
    spawnDustParticles();     // Polvo lunar
}
```

---

## ?? Resumen de Controles Completos

```
???????????????????????????????????????????????????????
?  MOVIMIENTO                                         ?
???????????????????????????????????????????????????????
?  W / ?              ?  Caminar adelante            ?
?  S / ?              ?  Caminar atrás               ?
?  A / ?              ?  Girar izquierda             ?
?  D / ?              ?  Girar derecha               ?
???????????????????????????????????????????????????????
?  SPRINT (NUEVO)                                     ?
???????????????????????????????????????????????????????
?  Shift + W / ?      ?  CORRER adelante (2.5x)      ?
?  Shift + S / ?      ?  CORRER atrás (2.5x)         ?
???????????????????????????????????????????????????????
?  CÁMARA                                             ?
???????????????????????????????????????????????????????
?  C                  ?  Cambiar 1ª ? 3ª persona     ?
?  Mouse Horizontal   ?  Rotar personaje             ?
?  Scroll (1ª pers.)  ?  Mirar ? (±50°)              ?
?  Scroll (3ª pers.)  ?  Zoom (1.0-15.0)             ?
???????????????????????????????????????????????????????
?  OTROS                                              ?
???????????????????????????????????????????????????????
?  L                  ?  Toggle indicadores de luz   ?
?  M / N / B          ?  Modos debug                 ?
?  ESC                ?  Salir                       ?
???????????????????????????????????????????????????????
```

---

## ? Conclusión

El sistema de sprint añade:
- ? **Movilidad mejorada** para el jugador
- ? **Control flexible** según la situación
- ? **Implementación simple** y eficiente
- ? **Compatible** con todas las características existentes
- ? **Personalizable** fácilmente

¡Disfruta explorando la superficie lunar a mayor velocidad! ?????
