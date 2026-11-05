#include "../common/core.h"
extern const topic_t help_topics[];
extern const topic_t fm_u1[], fm_u2[], fm_u3[], fm_u4[], fm_u6[];
extern const int fm_u1_n, fm_u2_n, fm_u3_n, fm_u4_n, fm_u6_n;
void app_main(void) {
    static unit_t units[6];
    units[0] = (unit_t){"Unit 1: Further Pure A", fm_u1, fm_u1_n};
    units[1] = (unit_t){"Unit 2: Further Statistics A", fm_u2, fm_u2_n};
    units[2] = (unit_t){"Unit 3: Further Mechanics A", fm_u3, fm_u3_n};
    units[3] = (unit_t){"Unit 4: Further Pure B", fm_u4, fm_u4_n};
    units[4] = (unit_t){"Unit 6: Further Mechanics B", fm_u6, fm_u6_n};
    units[5] = (unit_t){"Help: keys", help_topics, 1};
    app_run("Further Maths (WJEC)", units, 6);
}
