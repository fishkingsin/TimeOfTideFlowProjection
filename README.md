# TimeOfTideFlowProjection

## Creative Control Guide

Welcome to TimeOfTideFlowProjection! This tool empowers you to explore and shape dynamic visual effects in real time. Use the GUI and hotkeys to experiment, learn, and create unique fluid and particle visuals.

---

## Hotkeys Reference

| Key | Function           |
|-----|--------------------|
| F   | Toggle fullscreen  |
| G   | Show/hide GUI      |
| C   | Draw camera        |
| M   | Draw mouse / Draw actor |
| P   | Draw particles     |
| R   | Reset all settings |

Use these hotkeys to quickly toggle views and reset your canvas as you experiment.

---

## Mastering the GUI: Manipulating Visual Effects

The GUI exposes a wide range of parameters. Adjusting these will immediately influence the simulation and its appearance. Here’s how you can creatively control the effect:

### Visualization
- **Mode & Name**: Switch between different visualization styles (e.g., "fluid density") to explore new looks.
- **Scale**: Increase for more intense visuals, decrease for subtlety.
- **Show Scalar**: Toggle to reveal or hide scalar fields.

### Optical Flow
- **Threshold & Boost**: Lower threshold to make the system more sensitive to motion. Boost amplifies detected movement—great for energetic effects.

### Velocity & Density Bridges
- **Trail**: Higher values create longer motion trails.
- **Blur**: Softens the effect, blending motion.
- **Speed**: Controls how quickly the effect propagates.
- **Saturation** (Density): Boost for vivid color, reduce for muted tones.

### Temperature Bridge
- **Trail, Blur, Speed**: Similar to above, but influences thermal effects—try negative speed for reversed flows.

### Fluid Dynamics
- **Speed**: Controls how fast the fluid moves.
- **Dissipation**: Higher values make effects fade faster; lower values let them linger.
- **Viscosity**: Increase for thicker, slower-moving fluid.
- **Vorticity**: Adds swirling, turbulent motion—raise for more chaos.
- **Smoke Buoyancy**: Adjust buoyancy and weight for rising or sinking smoke effects.

### Particles
- **Speed**: Particle movement speed.
- **Birth Chance**: Probability of new particles spawning—higher for denser visuals.
- **Lifespan**: How long particles persist.
- **Size & Spread**: Control particle size and variation for texture.
- **Twinkle Speed**: Adds flicker—experiment for sparkling effects.
- **Gravity**: Set to nonzero for falling or rising particles.

### Mouse Interaction
- **Density & Velocity**: Control how your mouse injects energy or matter into the simulation.
- **Radius**: Affects the area of influence.
- **Color**: Change to paint with different hues.

---

## Network/Receiver Settings

- **Zactrack Receiver**: For advanced users, adjust port and axis inversion to match your tracking setup.

---

## Tips for Creative Exploration

- **Experiment**: Try extreme values to discover unexpected effects.
- **Combine**: Adjust multiple parameters together for complex results.
- **Reset**: Use `R` to return to defaults if things get wild.
- **Observe**: Small tweaks can have big visual impact—move sliders slowly and watch the changes.

---

## Getting Started

1. Launch the application.
2. Press `G` to show the GUI.
3. Adjust parameters and observe the effect in real time.
4. Use hotkeys to toggle features and reset as needed.

---

## Further Learning

- Dive into `bin/data/settings.xml` for default values and advanced tweaks.
- Explore the source code for deeper customization.

---

Unleash your creativity and enjoy exploring the endless possibilities of TimeOfTideFlowProjection!
