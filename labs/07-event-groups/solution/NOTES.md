# Lab 07 solution notes

## xEventGroupSync

Each task sets its bit and waits until **all** bits in `SYNC_BITS` are set, then those wait bits are cleared atomically — a rendezvous (“barrier”). `xEventGroupWaitBits` instead supports **wait-any** or **wait-all** without automatically clearing on exit.