void desk_set_keys(char **k, int n);
void app_main(void);
int main(int argc, char **argv) { desk_set_keys(argv + 1, argc - 1); app_main(); return 0; }
