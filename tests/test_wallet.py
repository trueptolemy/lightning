from decimal import Decimal
from fixtures import *  # noqa: F401,F403
from flaky import flaky  # noqa: F401
from lightning import RpcError, Millisatoshi
from utils import only_one, wait_for

import pytest
import time


def test_withdraw(node_factory, bitcoind):
    amount = 1000000
    # Don't get any funds from previous runs.
    l1 = node_factory.get_node(random_hsm=True)
    l2 = node_factory.get_node(random_hsm=True)
    addr = l1.rpc.newaddr()['bech32']

    # Add some funds to withdraw later
    for i in range(10):
        l1.bitcoin.rpc.sendtoaddress(addr, amount / 10**8 + 0.01)

    bitcoind.generate_block(1)
    wait_for(lambda: len(l1.rpc.listfunds()['outputs']) == 10)

    # Reach around into the db to check that outputs were added
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=0')[0]['c'] == 10

    waddr = l1.bitcoin.rpc.getnewaddress()
    # Now attempt to withdraw some (making sure we collect multiple inputs)
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'not an address', 'satoshi': amount}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': waddr, 'satoshi':'not an amount'}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': waddr, 'satoshi': -amount}])
    with pytest.raises(RpcError, match=r'Cannot afford transaction'):
        l1.rpc.withdraw([{'destination': waddr, 'satoshi': amount * 100}])

    out = l1.rpc.withdraw([{'destination': waddr, 'satoshi': 2 * amount}])

    # Make sure bitcoind received the withdrawal
    unspent = l1.bitcoin.rpc.listunspent(0)
    withdrawal = [u for u in unspent if u['txid'] == out['txid']]

    assert(withdrawal[0]['amount'] == Decimal('0.02'))

    # Now make sure two of them were marked as spent
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=2')[0]['c'] == 2

    # Now send some money to l2.
    # lightningd uses P2SH-P2WPKH
    waddr = l2.rpc.newaddr('bech32')['bech32']
    l1.rpc.withdraw([{'destination': waddr, 'satoshi': 2 * amount}])
    bitcoind.generate_block(1)

    # Make sure l2 received the withdrawal.
    wait_for(lambda: len(l2.rpc.listfunds()['outputs']) == 1)
    outputs = l2.db_query('SELECT value FROM outputs WHERE status=0;')
    assert only_one(outputs)['value'] == 2 * amount

    # Now make sure an additional two of them were marked as spent
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=2')[0]['c'] == 4

    # Simple test for withdrawal to P2WPKH
    # Address from: https://bc-2.jp/tools/bech32demo/index.html
    waddr = 'bcrt1qw508d6qejxtdg4y5r3zarvary0c5xw7kygt080'
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'xx1qw508d6qejxtdg4y5r3zarvary0c5xw7kxpjzsx', 'satoshi': 2 * amount}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1pw508d6qejxtdg4y5r3zarvary0c5xw7kdl9fad', 'satoshi': 2 * amount}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1qw508d6qejxtdg4y5r3zarvary0c5xw7kxxxxxx', 'satoshi': 2 * amount}])
    l1.rpc.withdraw([{'destination': waddr, 'satoshi': 2 * amount}])
    bitcoind.generate_block(1)
    # Now make sure additional two of them were marked as spent
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=2')[0]['c'] == 6

    # Simple test for withdrawal to P2WSH
    # Address from: https://bc-2.jp/tools/bech32demo/index.html
    waddr = 'bcrt1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3qzf4jry'
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'xx1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7', 'satoshi': 2 * amount}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination' :'tb1prp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3qsm03tq', 'satoshi': 2 * amount}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3qxxxxxx', 'satoshi': 2 * amount}])
    l1.rpc.withdraw([{'destination': waddr, 'satoshi': 2 * amount}])
    bitcoind.generate_block(1)
    # Now make sure additional two of them were marked as spent
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=2')[0]['c'] == 8

    # failure testing for invalid SegWit addresses, from BIP173
    # HRP character out of range
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': ' 1nwldj5', 'satoshi': 2 * amount}])
    # overall max length exceeded
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'an84characterslonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1569pvx',
                        'satoshi': 2 * amount}])
    # No separator character
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'pzry9x0s0muk', 'satoshi': 2 * amount}])
    # Empty HRP
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': '1pzry9x0s0muk', 'satoshi': 2 * amount}])
    # Invalid witness version
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'BC13W508D6QEJXTDG4Y5R3ZARVARY0C5XW7KN40WF2', 'satoshi': 2 * amount}])
    # Invalid program length for witness version 0 (per BIP141)
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'BC1QR508D6QEJXTDG4Y5R3ZARVARYV98GJ9P', 'satoshi': 2 * amount}])
    # Mixed case
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sL5k7', 'satoshi': 2 * amount}])
    # Non-zero padding in 8-to-5 conversion
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3pjxtptv', 'satoshi': 2 * amount}])

    # Should have 6 outputs available.
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=0')[0]['c'] == 6

    # Test withdrawal to self.
    l1.rpc.withdraw([{'destination': l1.rpc.newaddr('bech32')['bech32'], 'satoshi': 'all'}], minconf=0)
    bitcoind.generate_block(1)
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=0')[0]['c'] == 1

    l1.rpc.withdraw([{'destination': waddr, 'satoshi': 'all'}], minconf=0)
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=0')[0]['c'] == 0

    # This should fail, can't even afford fee.
    with pytest.raises(RpcError, match=r'Cannot afford transaction'):
        l1.rpc.withdraw([{'destination': waddr, 'satoshi': 'all'}])


def test_multiple_withdraw(node_factory, bitcoind):
    amount = 1000000
    # Don't get any funds from previous runs.
    l1 = node_factory.get_node(random_hsm=True)
    l2 = node_factory.get_node(random_hsm=True)
    addr = l1.rpc.newaddr()['bech32']

    # Add some funds to withdraw later
    for i in range(10):
        l1.bitcoin.rpc.sendtoaddress(addr, amount / 10**8 + 0.01)

    bitcoind.generate_block(1)
    wait_for(lambda: len(l1.rpc.listfunds()['outputs']) == 10)

    # Reach around into the db to check that outputs were added
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=0')[0]['c'] == 10

    waddr1 = l1.bitcoin.rpc.getnewaddress()
    waddr2 = l2.bitcoin.rpc.getnewaddress()
    amount1 = amount/2 + 500
    amonut2 = amount/2 - 500
    # Now attempt to withdraw some (making sure we collect multiple inputs)
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'not an address', 'satoshi': amount1},
                         {'destination': waddr1, 'satoshi': amount2}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': waddr1, 'satoshi':'not an amount'},
                         {'destination': waddr2, 'satoshi': amount1}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': waddr1, 'satoshi': -amount1},
                         {'destination': waddr2, 'satoshi': amount2}])
    with pytest.raises(RpcError, match=r'Cannot afford transaction'):
        l1.rpc.withdraw([{'destination': waddr1, 'satoshi': amount1 * 100},
                         {'destination': waddr2, 'satoshi': amount2 * 100}])

    out = l1.rpc.withdraw([{'destination': waddr1, 'satoshi': 2 * amount1},
                           {'destination': waddr2, 'satoshi': 2 * amount2}])

    # Make sure bitcoind received the withdrawal
    unspent = l1.bitcoin.rpc.listunspent(0)
    withdrawal = [u for u in unspent if u['txid'] == out['txid']]

    assert(withdrawal[0]['amount'] == Decimal('0.02'))

    # Now make sure two of them were marked as spent
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=2')[0]['c'] == 2

    # Now send some money to l2.
    # lightningd uses P2SH-P2WPKH
    waddr3 = l2.rpc.newaddr('bech32')['bech32']
    l1.rpc.withdraw([{'destination': waddr3, 'satoshi': 2 * amount1},
                     {'destination': waddr1, 'satoshi': 2 * amount2}])
    bitcoind.generate_block(1)

    # Make sure l2 received the withdrawal.
    wait_for(lambda: len(l2.rpc.listfunds()['outputs']) == 1)
    outputs = l2.db_query('SELECT value FROM outputs WHERE status=0;')
    assert only_one(outputs)['value'] == 2 * amount1

    # Now make sure an additional two of them were marked as spent
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=2')[0]['c'] == 4

    # Simple test for withdrawal to P2WPKH
    # Address from: https://bc-2.jp/tools/bech32demo/index.html
    waddr4 = 'bcrt1qw508d6qejxtdg4y5r3zarvary0c5xw7kygt080'
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'xx1qw508d6qejxtdg4y5r3zarvary0c5xw7kxpjzsx', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1pw508d6qejxtdg4y5r3zarvary0c5xw7kdl9fad', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1qw508d6qejxtdg4y5r3zarvary0c5xw7kxxxxxx', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    l1.rpc.withdraw([{'destination': waddr1, 'satoshi': amount},
                     {'destination': waddr4, 'satoshi': amount}])
    bitcoind.generate_block(1)
    # Now make sure additional two of them were marked as spent
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=2')[0]['c'] == 6

    # Simple test for withdrawal to P2WSH
    # Address from: https://bc-2.jp/tools/bech32demo/index.html
    waddr5 = 'bcrt1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3qzf4jry'
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'xx1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination' :'tb1prp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3qsm03tq', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3qxxxxxx', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    l1.rpc.withdraw([{'destination': waddr5, 'satoshi': amount},
                     {'destination': waddr1, 'satoshi': amount}])
    bitcoind.generate_block(1)
    # Now make sure additional two of them were marked as spent
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=2')[0]['c'] == 8

    # failure testing for invalid SegWit addresses, from BIP173
    # HRP character out of range
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': ' 1nwldj5', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    # overall max length exceeded
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'an84characterslonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1569pvx',
                        'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    # No separator character
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'pzry9x0s0muk', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    # Empty HRP
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': '1pzry9x0s0muk', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    # Invalid witness version
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'BC13W508D6QEJXTDG4Y5R3ZARVARY0C5XW7KN40WF2', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    # Invalid program length for witness version 0 (per BIP141)
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'BC1QR508D6QEJXTDG4Y5R3ZARVARYV98GJ9P', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    # Mixed case
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sL5k7', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])
    # Non-zero padding in 8-to-5 conversion
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': 'tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3pjxtptv', 'satoshi': amount},
                         {'destination': waddr1, 'satoshi': amount}])

    # Should have 6 outputs available.
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=0')[0]['c'] == 6

    # Test withdrawal "all" with two outputs
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': waddr1, 'satoshi': 'all'},
                         {'destination': waddr2, 'satoshi': amount}])

    # Test withdrawal to self.
    l1.rpc.withdraw([{'destination': l1.rpc.newaddr('bech32')['bech32'], 'satoshi': 'all'}], minconf=0)
    bitcoind.generate_block(1)
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=0')[0]['c'] == 1

    l1.rpc.withdraw([{'destination': waddr5, 'satoshi': 'all'}], minconf=0)
    assert l1.db_query('SELECT COUNT(*) as c FROM outputs WHERE status=0')[0]['c'] == 0

    # This should fail, can't even afford fee.
    with pytest.raises(RpcError, match=r'Cannot afford transaction'):
        l1.rpc.withdraw([{'destination': waddr5, 'satoshi': 'all'}])


def test_minconf_withdraw(node_factory, bitcoind):
    """Issue 2518: ensure that ridiculous confirmation levels don't overflow

    The number of confirmations is used to compute a maximum height that is to
    be accepted. If the current height is smaller than the number of
    confirmations we wrap around and just select everything. The fix is to
    clamp the maxheight parameter to a positive small number.

    """
    amount = 1000000
    # Don't get any funds from previous runs.
    l1 = node_factory.get_node(random_hsm=True)
    addr = l1.rpc.newaddr()['bech32']

    # Add some funds to withdraw later
    for i in range(10):
        l1.bitcoin.rpc.sendtoaddress(addr, amount / 10**8 + 0.01)

    bitcoind.generate_block(1)

    wait_for(lambda: len(l1.rpc.listfunds()['outputs']) == 10)
    with pytest.raises(RpcError):
        l1.rpc.withdraw([{'destination': addr, 'satoshi': 10000}], feerate='normal', minconf=9999999)


def test_addfunds_from_block(node_factory, bitcoind):
    """Send funds to the daemon without telling it explicitly
    """
    # Previous runs with same bitcoind can leave funds!
    l1 = node_factory.get_node(random_hsm=True)

    addr = l1.rpc.newaddr()['bech32']
    bitcoind.rpc.sendtoaddress(addr, 0.1)
    bitcoind.generate_block(1)

    wait_for(lambda: len(l1.rpc.listfunds()['outputs']) == 1)

    outputs = l1.db_query('SELECT value FROM outputs WHERE status=0;')
    assert only_one(outputs)['value'] == 10000000

    # The address we detect must match what was paid to.
    output = only_one(l1.rpc.listfunds()['outputs'])
    assert output['address'] == addr

    # Send all our money to a P2WPKH address this time.
    addr = l1.rpc.newaddr("bech32")['bech32']
    l1.rpc.withdraw([{'destination': addr, 'satoshi': "all"}])
    bitcoind.generate_block(1)
    time.sleep(1)

    # The address we detect must match what was paid to.
    output = only_one(l1.rpc.listfunds()['outputs'])
    assert output['address'] == addr


def test_txprepare(node_factory, bitcoind):
    amount = 1000000
    l1 = node_factory.get_node(random_hsm=True)

    # Add some funds to withdraw later: both bech32 and p2sh
    for i in range(5):
        bitcoind.rpc.sendtoaddress(l1.rpc.newaddr()['bech32'],
                                   amount / 10**8)
        bitcoind.rpc.sendtoaddress(l1.rpc.newaddr('p2sh-segwit')['p2sh-segwit'],
                                   amount / 10**8)

    bitcoind.generate_block(1)
    wait_for(lambda: len(l1.rpc.listfunds()['outputs']) == 10)

    prep = l1.rpc.txprepare([{'destination': 'bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg',
                            'satoshi': Millisatoshi(amount * 3 * 1000)}])
    decode = bitcoind.rpc.decoderawtransaction(prep['unsigned_tx'])
    assert decode['txid'] == prep['txid']
    # 4 inputs, 2 outputs.
    assert len(decode['vin']) == 4
    assert len(decode['vout']) == 2

    # One output will be correct.
    if decode['vout'][0]['value'] == Decimal(amount * 3) / 10**8:
        outnum = 0
        changenum = 1
    elif decode['vout'][1]['value'] == Decimal(amount * 3) / 10**8:
        outnum = 1
        changenum = 0
    else:
        assert False

    assert decode['vout'][outnum]['scriptPubKey']['type'] == 'witness_v0_keyhash'
    assert decode['vout'][outnum]['scriptPubKey']['addresses'] == ['bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg']

    assert decode['vout'][changenum]['scriptPubKey']['type'] == 'witness_v0_keyhash'

    # Now prepare one with no change.
    prep2 = l1.rpc.txprepare([{'destination': 'bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg',
                             'satoshi': 'all'}])
    decode = bitcoind.rpc.decoderawtransaction(prep2['unsigned_tx'])
    assert decode['txid'] == prep2['txid']
    # 6 inputs, 1 outputs.
    assert len(decode['vin']) == 6
    assert len(decode['vout']) == 1

    # Some fees will be paid.
    assert decode['vout'][0]['value'] < Decimal(amount * 6) / 10**8
    assert decode['vout'][0]['value'] > Decimal(amount * 6) / 10**8 - Decimal(0.0002)
    assert decode['vout'][0]['scriptPubKey']['type'] == 'witness_v0_keyhash'
    assert decode['vout'][0]['scriptPubKey']['addresses'] == ['bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg']

    # If I cancel the first one, I can get those first 4 outputs.
    discard = l1.rpc.txdiscard(prep['txid'])
    assert discard['txid'] == prep['txid']
    assert discard['unsigned_tx'] == prep['unsigned_tx']

    prep3 = l1.rpc.txprepare([{'destination': 'bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg',
                             'satoshi': 'all'}])
    decode = bitcoind.rpc.decoderawtransaction(prep3['unsigned_tx'])
    assert decode['txid'] == prep3['txid']
    # 4 inputs, 1 outputs.
    assert len(decode['vin']) == 4
    assert len(decode['vout']) == 1

    # Some fees will be taken
    assert decode['vout'][0]['value'] < Decimal(amount * 4) / 10**8
    assert decode['vout'][0]['value'] > Decimal(amount * 4) / 10**8 - Decimal(0.0002)
    assert decode['vout'][0]['scriptPubKey']['type'] == 'witness_v0_keyhash'
    assert decode['vout'][0]['scriptPubKey']['addresses'] == ['bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg']

    # Cannot discard twice.
    with pytest.raises(RpcError, match=r'not an unreleased txid'):
        l1.rpc.txdiscard(prep['txid'])

    # Discard everything, we should now spend all inputs.
    l1.rpc.txdiscard(prep2['txid'])
    l1.rpc.txdiscard(prep3['txid'])
    prep4 = l1.rpc.txprepare([{'destination': 'bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg',
                             'satoshi':'all'}])
    decode = bitcoind.rpc.decoderawtransaction(prep4['unsigned_tx'])
    assert decode['txid'] == prep4['txid']
    # 10 inputs, 1 outputs.
    assert len(decode['vin']) == 10
    assert len(decode['vout']) == 1

    # Some fees will be taken
    assert decode['vout'][0]['value'] < Decimal(amount * 10) / 10**8
    assert decode['vout'][0]['value'] > Decimal(amount * 10) / 10**8 - Decimal(0.0003)
    assert decode['vout'][0]['scriptPubKey']['type'] == 'witness_v0_keyhash'
    assert decode['vout'][0]['scriptPubKey']['addresses'] == ['bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg']


def test_txsend(node_factory, bitcoind):
    amount = 1000000
    l1 = node_factory.get_node(random_hsm=True)

    # Add some funds to withdraw later: both bech32 and p2sh
    for i in range(5):
        bitcoind.rpc.sendtoaddress(l1.rpc.newaddr()['bech32'],
                                   amount / 10**8)
        bitcoind.rpc.sendtoaddress(l1.rpc.newaddr('p2sh-segwit')['p2sh-segwit'],
                                   amount / 10**8)
    bitcoind.generate_block(1)
    wait_for(lambda: len(l1.rpc.listfunds()['outputs']) == 10)

    prep = l1.rpc.txprepare([{'destination': 'bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg',
                            'satoshi': Millisatoshi(amount * 3 * 1000)}])
    out = l1.rpc.txsend(prep['txid'])

    # Cannot discard after send!
    with pytest.raises(RpcError, match=r'not an unreleased txid'):
        l1.rpc.txdiscard(prep['txid'])

    wait_for(lambda: prep['txid'] in bitcoind.rpc.getrawmempool())

    # Signed tx should have same txid
    decode = bitcoind.rpc.decoderawtransaction(out['tx'])
    assert decode['txid'] == prep['txid']

    bitcoind.generate_block(1)

    # Change output should appear.
    if decode['vout'][0]['value'] == Decimal(amount * 3) / 10**8:
        changenum = 1
    elif decode['vout'][1]['value'] == Decimal(amount * 3) / 10**8:
        changenum = 0
    else:
        assert False

    # Those spent outputs are gone, but change output has arrived.
    wait_for(lambda: len(l1.rpc.listfunds()['outputs']) == 10 - len(decode['vin']) + 1)

    # Change address should appear in listfunds()
    assert decode['vout'][changenum]['scriptPubKey']['addresses'][0] in [f['address'] for f in l1.rpc.listfunds()['outputs']]


def test_txprepare_restart(node_factory, bitcoind):
    amount = 1000000
    l1 = node_factory.get_node(may_fail=True)

    # Add some funds to withdraw later: both bech32 and p2sh
    for i in range(5):
        bitcoind.rpc.sendtoaddress(l1.rpc.newaddr()['bech32'],
                                   amount / 10**8)
        bitcoind.rpc.sendtoaddress(l1.rpc.newaddr('p2sh-segwit')['p2sh-segwit'],
                                   amount / 10**8)
    bitcoind.generate_block(1)
    wait_for(lambda: [o['status'] for o in l1.rpc.listfunds()['outputs']] == ['confirmed'] * 10)

    prep = l1.rpc.txprepare([{'destination': 'bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg',
                            'satoshi': 'all'}])
    decode = bitcoind.rpc.decoderawtransaction(prep['unsigned_tx'])
    assert decode['txid'] == prep['txid']
    # All 10 inputs
    assert len(decode['vin']) == 10

    # L1 will forget all about it.
    l1.restart()

    # It goes backwards in blockchain just in case there was a reorg.  Wait.
    wait_for(lambda: [o['status'] for o in l1.rpc.listfunds()['outputs']] == ['confirmed'] * 10)

    with pytest.raises(RpcError, match=r'not an unreleased txid'):
        l1.rpc.txdiscard(prep['txid'])

    prep = l1.rpc.txprepare([{'destination': 'bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg',
                            'satoshi': 'all'}])

    decode = bitcoind.rpc.decoderawtransaction(prep['unsigned_tx'])
    assert decode['txid'] == prep['txid']
    # All 10 inputs
    assert len(decode['vin']) == 10

    # This will also work if we simply kill it.
    l1.restart(clean=False)

    # It goes backwards in blockchain just in case there was a reorg.  Wait.
    wait_for(lambda: [o['status'] for o in l1.rpc.listfunds()['outputs']] == ['confirmed'] * 10)

    # It should have logged this for each output.
    for i in decode['vin']:
        assert l1.daemon.is_in_log('wallet: reserved output {}/{} reset to available'.format(i['txid'], i['vout']))

    prep = l1.rpc.txprepare([{'destination': 'bcrt1qeyyk6sl5pr49ycpqyckvmttus5ttj25pd0zpvg',
                            'satoshi': 'all'}])
    decode = bitcoind.rpc.decoderawtransaction(prep['unsigned_tx'])
    assert decode['txid'] == prep['txid']
    # All 10 inputs
    assert len(decode['vin']) == 10
