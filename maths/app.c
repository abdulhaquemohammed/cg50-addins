#include "../common/core.h"
extern const topic_t help_topics[];
extern const topic_t m_u1[], m_u2[], m_u3[], m_u4[];
extern const int m_u1_n, m_u2_n, m_u3_n, m_u4_n;
void app_main(void) {
    static unit_t units[5];
    units[0] = (unit_t){"Unit 1: Pure Mathematics A", m_u1, m_u1_n};
    units[1] = (unit_t){"Unit 2: Applied Mathematics A", m_u2, m_u2_n};
    units[2] = (unit_t){"Unit 3: Pure Mathematics B", m_u3, m_u3_n};
    units[3] = (unit_t){"Unit 4: Applied Mathematics B", m_u4, m_u4_n};
    units[4] = (unit_t){"Help: keys", help_topics, 1};
    app_run("A Level Maths (WJEC)", units, 5);
}
