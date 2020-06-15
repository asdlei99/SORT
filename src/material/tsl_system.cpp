/*
    This file is a part of SORT(Simple Open Ray Tracing), an open-source cross
    platform physically based renderer.

    Copyright (c) 2011-2020 by Jiayin Cao - All rights reserved.

    SORT is a free software written for educational purpose. Anyone can distribute
    or modify it under the the terms of the GNU General Public License Version 3 as
    published by the Free Software Foundation. However, there is NO warranty that
    all components are functional in a perfect manner. Without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License along with
    this program. If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
 */

#include <algorithm>
#include <vector>
#include <shading_context.h>
#include "tsl_system.h"
#include "texture_system.h"
#include "core/profile.h"
#include "core/thread.h"
#include "core/globalconfig.h"
#include "math/interaction.h"
#include "scatteringevent/scatteringevent.h"
#include "medium/medium.h"

using namespace Tsl_Namespace;

IMPLEMENT_TSLGLOBAL_BEGIN()
IMPLEMENT_TSLGLOBAL_VAR(float3, normal)
IMPLEMENT_TSLGLOBAL_END()

class SORT_Shader_MemoryAllocator : public Tsl_Namespace::MemoryAllocator {
    void* allocate(unsigned int size) const override {
        return SORT_MALLOC_ARRAY(char, size);
    }
};

// Tsl shading system
static SORT_Shader_MemoryAllocator     g_shader_memory_allocator;
static std::unique_ptr<ShadingSystem>  g_shadingsys = nullptr;
static std::vector<ShadingContext*>    g_contexts;
// static std::vector<ShadingContextWrapper>   g_shadingContexts;

std::unique_ptr<ShadingSystem>  MakeShadingSystem() {
     return std::move(std::make_unique<ShadingSystem>());
}

ShaderUnitTemplate*     BuildShader(const std::string& shader_name, const std::string& shader_source) {
    return g_contexts[ThreadId()]->compile_shader_unit_template(shader_name, shader_source.c_str());
};

ShaderGroupTemplate*    BeginShaderGroup(const std::string& group_name) {
    return g_contexts[ThreadId()]->begin_shader_group_template(group_name);
}

bool EndShaderGroup(ShaderGroupTemplate* sg) {
    return g_contexts[ThreadId()]->end_shader_group_template(sg);
}

bool ResolveShaderInstance(ShaderInstance* si) {
    return g_contexts[ThreadId()]->resolve_shader_instance(si);
}

void ExecuteSurfaceShader( Tsl_Namespace::ShaderInstance* shader , ScatteringEvent& se ){
    const SurfaceInteraction& intersection = se.GetInteraction();
    TslGlobal global;
    global.normal = make_float3(intersection.normal.x, intersection.normal.y, intersection.normal.z);

    // shader execution
    ClosureTreeNodeBase* closure = nullptr;
    auto raw_function = (void(*)(ClosureTreeNodeBase**, TslGlobal*))shader->get_function();
    raw_function(&closure, &global);

    // parse the surface shader
    ProcessSurfaceClosure(closure, Tsl_Namespace::make_float3(1.0f, 1.0f, 1.0f) , se );
}

// void ExecuteVolumeShader(OSL::ShaderGroup* shader, const MediumInteraction& mi, MediumStack& ms, const SE_Interaction flag, const MaterialBase* material ) {
//     ShaderGlobals shaderglobals;
//     memset(&shaderglobals, 0, sizeof(shaderglobals));
//     shaderglobals.P = Vec3(mi.intersect.x, mi.intersect.y, mi.intersect.z);

//     // objdata points to world to local volume transform
//     auto thread_info = reinterpret_cast<SORTTextureThreadInfo*>(g_textureSystem.get_perthread_info());
//     if (mi.mesh)
//         thread_info->mesh = mi.mesh;

//     g_shadingsys->execute(g_contexts[ThreadId()], *shader, shaderglobals);

//     ProcessVolumeClosure(shaderglobals.Ci, Color3(1.0f), ms, flag, material, mi.mesh);
// }

// void EvaluateVolumeSample(OSL::ShaderGroup* shader, const MediumInteraction& mi, MediumSample& ms) {
//     ShaderGlobals shaderglobals;
//     memset(&shaderglobals, 0, sizeof(shaderglobals));
//     shaderglobals.P = Vec3(mi.intersect.x, mi.intersect.y, mi.intersect.z);

//     // objdata points to world to local volume transform
//     auto thread_info = reinterpret_cast<SORTTextureThreadInfo*>(g_textureSystem.get_perthread_info());
//     if (mi.mesh)
//         thread_info->mesh = mi.mesh;

//     g_shadingsys->execute(g_contexts[ThreadId()], *shader, shaderglobals);

//     ProcessVolumeClosure(shaderglobals.Ci, Color3(1.0f), ms);
// }

// Spectrum EvaluateTransparency( OSL::ShaderGroup* shader , const SurfaceInteraction& intersection ){
//     ShaderGlobals shaderglobals;
//     shaderglobals.P = Vec3( intersection.intersect.x , intersection.intersect.y , intersection.intersect.z );
//     shaderglobals.u = intersection.u;
//     shaderglobals.v = intersection.v;
//     shaderglobals.N = Vec3( intersection.normal.x , intersection.normal.y , intersection.normal.z );
//     shaderglobals.Ng = Vec3( intersection.gnormal.x , intersection.gnormal.y , intersection.gnormal.z );
//     shaderglobals.I = Vec3( intersection.view.x , intersection.view.y , intersection.view.z );
//     shaderglobals.dPdu = Vec3( 0.0f );
//     shaderglobals.dPdu = Vec3( 0.0f );
//     g_shadingsys->execute(g_contexts[ ThreadId() ], *shader, shaderglobals);

//     const auto opacity = ProcessOpacity( shaderglobals.Ci , Color3( 1.0f ) );
//     return Spectrum( 1.0f - opacity ).Clamp( 0.0f , 1.0f );
// }

// void ShadingContextWrapper::DestroyContext(OSL::ShadingSystem* shadingsys) {
//     shadingsys->release_context(ctx);
//     shadingsys->destroy_thread_info(thread_info);
// }

// OSL::ShadingContext* ShadingContextWrapper::GetShadingContext(OSL::ShadingSystem* shadingsys){
//     thread_info = shadingsys->create_thread_info();
//     return ctx = shadingsys->get_context(thread_info);
// }

void CreateTSLThreadContexts(){
    // make global tsl shading system
    g_shadingsys = MakeShadingSystem();

    // shader memory allocator
    g_shadingsys->register_memory_allocator(&g_shader_memory_allocator);

    // register all closures
    RegisterClosures(g_shadingsys.get());

    // register tsl global, this may need to be per-shader data later for more flexibility
    TslGlobal::RegisterGlobal(*g_shadingsys);

    // allocate shading context for each thread
    g_contexts.resize( g_threadCnt );
    for (auto i = 0u; i < g_threadCnt; ++i)
        g_contexts[i] = g_shadingsys->make_shading_context();
}

 void DestroyTSLThreadContexts(){
     // to be implemented
 }