#ifndef STATS_H
#define STATS_H
double norm_cdf(double z);
double norm_pdf(double z);
double norm_inv(double p);
double gammap(double a, double x);
double chi2_cdf(double x, int k);
double chi2_inv(double p, int k);
double betai(double a, double b, double x);
double t_cdf(double t, int v);
double t_inv(double p, int v);
double pois_pmf(int r, double l);
double pois_cdf(int r, double l);
double binom_pmf(int r, int n, double p);
double binom_cdf(int r, int n, double p);
double pmcc_crit(int n, double alpha);
double spearman_p_upper(int n, double rs, int *exact);
int parse_list(const char *s, double *v, int max);
/* UI: enter a list of numbers */
int in_list(const char *label, double *v, int max);
#endif
