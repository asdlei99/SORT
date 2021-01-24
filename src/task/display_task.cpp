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

#include <atomic>
#include "display_task.h"
#include "core/globalconfig.h"
#include "core/display_mgr.h"

extern std::atomic<int> g_render_task_cnt;

Display_Task::Display_Task(const char* name, unsigned int priority, const Task::Task_Container& dependencies) :Task(name, priority, dependencies) {
}

void Display_Task::Execute() {
    while (g_render_task_cnt > 0) {
        DisplayManager::GetSingleton().ProcessDisplayQueue();
        std::this_thread::yield();
    }

    // terminator is only needed in blender mode
    if (g_blenderMode) {
        std::shared_ptr<TerminateIndicator> terminator = std::make_shared<TerminateIndicator>();
        DisplayManager::GetSingleton().QueueDisplayItem(terminator);
    }

    // make sure flush all display items before quiting
    DisplayManager::GetSingleton().ProcessDisplayQueue(-1);
}