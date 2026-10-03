# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Seetron is a Windows-only C++17 3D engine (Direct3D 11) by Nicolas Arques, plus a Qt-based editor shell. Every engine type is prefixed `LX`.

## Build

- Visual Studio / MSBuild only: `x64` platform, `Debug` and `Release` configurations, toolset `v142` (VS2019). There are no tests, no linter, and no CMake.
- **No `.sln` is committed**, and neither is the `External/` dependency folder (e.g. `External/FreeImage3170`, referenced by `Seetron.props`). The projects cannot build from a fresh clone alone, and nothing can be built in a Linux container.
- `Seetron.props` is the shared property sheet: include paths `.\;..\;..\..\;..\External`, output to `$(SolutionDir)Bin\$(Platform)\$(Configuration)\`, intermediates to `$(SolutionDir)Build\$(ProjectName)\...`. Because the repo root is on the include path, cross-project includes are written like `#include "LXCore/LXObject.h"`.
- Build example, from a VS Developer prompt with a solution in place: `msbuild LXEngine\LXEngine.vcxproj /p:Configuration=Debug /p:Platform=x64`.
- **When you add or remove a source file, update the matching `.vcxproj` by hand** (`ClCompile` / `ClInclude` items). Files are listed explicitly.

## Projects

| Project | Type | Notes |
|---|---|---|
| `LXCore/` | DLL (`LXCORE_API`, `LXCORE_EXPORTS`) | New low-level layer. `LXObject` and `LXString` were recently moved here from the engine. More base code is meant to follow. |
| `LXEngine/` | DLL (`LXENGINE_API`, `LXENGINE_EXPORTS`) | The engine itself, in one flat directory of about 200 `.cpp`/`.h` pairs. References LXCore. |
| `Programs/SeetonEditor/` | Qt app (QtMsBuild, `msvc2019_64`, core/gui/widgets) | Still a stub: a blank `QMainWindow`. It does not link the engine yet. |

- Each DLL uses a precompiled header (`pch.h`), included first in every `.cpp`.
- `LXEngine/LXInclude.h` is the umbrella header for the engine pch. It pulls in `LXConfig.h`, `LXCore/LXInclude.h`, STL, `<windows.h>`, the math types (`LXVec*`, `LXMatrix`) and the `CHK` debug macros (`LXDebug.h`).
- `LXConfig.h` holds the compile-time feature switches: `LX_WINDOWS_MUTEX`, `LX_LOADING_THREAD`, `LX_TRACE_OBJECTS`, `LX_COUNTERS`, `LX_MAX_LODS` and so on. It also defines the asset file extensions (`.seproj`, `.smat`, `.smi`, `.stex`, `.smesh`, `.sebin`, `.sgtex`, `.anim`). `LX_EDITOR`, `LX_DEV` and `LX_ENGINE_STATIC` are meant to be set as project defines.

## Engine architecture (LXEngine)

**Object model.** `LXObject` (in LXCore; non-copyable) → `LXSmartObject` → `LXActor` / `LXComponent` / assets.
- `LXSmartObject` carries the reflection and property system. Members are exposed with `DefineProperty*()`, usually in the constructor, using a `LXPropertyID` (`LXPropertyIdentifiers.h`) or an automatic ID. The property code lives in `LXProperty*.h/.cpp`; `LXPropertyTemplate.h` and `LXPropertyAsset.h` were recently split out.
- Properties drive several systems:
  - XML serialization (`LXMSXMLNode`, `LXXMLDocument`)
  - undo/redo (`LXCommandModifyProperty` through `LXCommandManager`)
  - editor UI

**Singleton engine and managers.** `LXEngine::CreateEngine()` / `GetEngine()` own the managers: document, viewport, command, property, selection, asset, event, message, settings, stats, script, and the actor factory. `LXProject` is the loaded document and holds the `LXScene`.

**Threading: MainThread and RenderThread with a sync point.** See `LXEngine::Run()` in `LXEngine.cpp`.
1. The main thread waits for the render thread's end-of-frame event (`LXSyncEvent`).
2. While the render thread is blocked, the main thread runs `_syncTasks`. This is the only safe window to hand game-side state to the renderer.
3. It then restarts the render thread, runs `_mainTasks`, the message manager and `LXScene::Run()`.

When `LXRenderer::gUseRenderThread` is false, it calls `Render_MainThread()` instead.

To cross threads, use `LXEngine::EnqueueTask` / `EnqueueInvokeDelegate` and `LXRenderer::EnqueueTask`. Don't touch the other thread's state directly. `CHK(IsMainThread())`-style asserts guard this. Functions suffixed `_RT` run on the render thread.

**Actor → renderer bridge.** An actor never talks to D3D. It calls `InvalidateRenderState(ERenderClusterRole flags)`, which leads to `LXRenderData` and then `LXRenderer::UpdateActor`. The renderer turns primitives into `LXRenderCluster`s (`LXRenderClusterManager`). Pairs of the form `LXPrimitive` → `LXPrimitiveD3D11` show the split: the plain class is CPU data, and the `*D3D11` class is the device-side mirror, owned by the render side.

**Render pipeline.** `LXRenderPipelineDeferred` chains `LXRenderPass*` subclasses:
- Depth
- Shadow
- GBuffer
- SSAO
- Lighting
- Transparency
- DepthOfField
- ToneMapping
- Downsample
- UI / Aux / DynamicTexture

Passes record into `LXRenderCommandList` (`LXRenderCommand*` / `LXRenderCommandDX11`). Render targets, textures, samplers and constant buffers each have a `*D3D11` class.

**Materials are node graphs compiled to HLSL.** These classes are involved:
- `LXGraphMaterial` and `LXGraphNodeMaterial` hold the node graph.
- `LXGraphTemplate` defines the node templates.
- `LXGraphMaterialToHLSLConverter::GenerateCode(material, ERenderPass, EShader)` turns the graph into HLSL source.
- `LXShaderManager` and `LXShaderFactory` compile the generated source and cache it.
- `LXMaterialD3D11` binds the result.
- `LXMaterialInstance` overrides parameters of a parent material.

Shaders are generated per render pass.

**Procedural and terrain.** These pieces exist in the engine:
- `LXComponentTerrainGenerator`
- `LXTerrain`
- `LXComponentMetaPatch`
- `LXProceduralTexture` (`.sgtex`)
- `LXActorForest` / `LXActorTree`
- `LXLODSystem`

## Conventions

- Every file starts with the `// This is a part of Seetron Engine / Copyright (c) Nicolas Arques` banner. Keep it on new files.
- Mark exported engine classes `LXENGINE_API` and exported core classes `LXCORE_API`.
- Naming is mixed (`m_` prefixes, a `_leading` underscore, and PascalCase members all appear). Match whatever the surrounding file uses.
- Assert with `CHK(...)` (`LXDebug.h`).
