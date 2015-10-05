/* This is actually  Tom Kelly's Scalable TCP (See http://www.deneholme.net/tom/scalable/)
*   It is implemented in tcp_scalable.c in the linux Kernel
*   We just changed some names to make it "ours" :)
*/

#include <linux/module.h>
#include <net/tcp.h>

/* These factors derived from the recommended values in the aer:
 * .01 and and 7/8. We use 50 instead of 100 to account for
 * delayed ack.
 */
#define TCP_TA_AI_CNT	50U
#define TCP_TA_MD_SCALE	3

static void tcp_TA_cong_avoid(struct sock *sk, u32 ack, u32 acked)
{
	struct tcp_sock *tp = tcp_sk(sk);

	if (!tcp_is_cwnd_limited(sk))
		return;

	if (tp->snd_cwnd <= tp->snd_ssthresh)
		tcp_slow_start(tp, acked);
	else
		tcp_cong_avoid_ai(tp, min(tp->snd_cwnd, TCP_TA_AI_CNT),
				  1);
}

static u32 tcp_TA_ssthresh(struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);

	return max(tp->snd_cwnd - (tp->snd_cwnd>>TCP_TA_MD_SCALE), 2U);
}

static struct tcp_congestion_ops tcp_TA __read_mostly = {
	.ssthresh	= tcp_TA_ssthresh,
	.cong_avoid	= tcp_TA_cong_avoid,

	.owner		= THIS_MODULE,
	.name		= "TA_FirstCCAlgorithm",
};

static int __init tcp_TA_register(void)
{
	return tcp_register_congestion_control(&tcp_TA);
}

static void __exit tcp_TA_unregister(void)
{
	tcp_unregister_congestion_control(&tcp_TA);
}

module_init(tcp_TA_register);
module_exit(tcp_TA_unregister);

MODULE_AUTHOR("Tubin and Amos");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("FirstCCAlgorithm");
