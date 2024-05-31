int cmp = BandingkanTanggal(tanggal_checkin, node->tiket.tanggal_checkin);
    if (cmp == 0) {
        return node;
    } else if (cmp < 0) {
        return cariTiket(node->left, tanggal_checkin);
    } else {
        return cariTiket(node->right, tanggal_checkin);
    }