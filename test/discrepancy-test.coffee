################################### REQUIRES ###################################

assert                     = require 'assert'
async                      = require 'async'

{
  Amount
  Meta
}                          = require 'ripple-lib'

testutils                  = require './testutils'

{
  pretty_json
  server_setup_teardown
}                          = require './batmans-belt'

#################################### CONFIG ####################################

config = testutils.init_config()

#################################### HELPERS ###################################

tx_blob_submit_factory = (remote, txn) ->
  (next) ->
    req = remote.request_submit()
    req.message.tx_blob = txn
    req.once 'error', -> assert false, "unexpected error submitting txns"
    req.once 'success', (m) ->
      remote.once 'ledger_closed', -> next()
      remote.ledger_accept()
    req.request()

offer_amounts = (fields) ->
  [Amount.from_json(fields.TakerPays),
   Amount.from_json(fields.TakerGets)]

executed_offers = (meta) ->
  offers = {}

  meta.nodes.forEach (n, i) ->
    if n.entryType == 'Offer'
      [prev_pays, prev_gets] = offer_amounts(n.fieldsPrev)
      [final_pays, final_gets] = offer_amounts(n.fields)

      summary=
        pays_executed: prev_pays.subtract(final_pays).to_text_full()
        gets_executed: prev_gets.subtract(final_gets).to_text_full()
        pays_final: final_pays.to_text_full()
        gets_final: final_gets.to_text_full()
        owner: n.fields.Account

      offers[n.ledgerIndex] = summary
  offers

build_tx_blob_submission_series = (remote, txns_to_submit) ->
  series = []
  while (txn = txns_to_submit.shift())
    series.push tx_blob_submit_factory remote, txn
  series

compute_xrp_discrepancy = (fee, meta) ->
  before = Amount.from_json(0)
  after = Amount.from_json(fee)

  meta.nodes.forEach (n, i) ->
    if n.entryType == 'AccountRoot'
      prev = n.fieldsPrev?.Balance || n.fields.Balance
      final  = n.fieldsFinal?.Balance || n.fields.Balance

      before = before.add(Amount.from_json(prev))
      after = after.add(Amount.from_json(final))

  before.subtract(after)

suite 'Discrepancy test', ->
  suite 'XRP Discrepancy', ->
    get_context = server_setup_teardown({server_opts: {ledger_file: 'ledger-6226713.json'}})
    test '01030E435C2EEBE2689FED7494BC159A4C9B98D0DF0B23F7DFCE223822237E8C', (done) ->
      {remote} = get_context()
      txns_to_submit = [
        # This is the nasty one ;)
        '1200002200020000240000124E61D5438D7EA4C680000000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD468400000000000000A69D4D3E7809B4814C8000000000000000000000000434E59000000000041C8BE2C0A6AA17471B9F6D0AF92AAB1C94D5A25732103FC5F96EA61889691EC7A56FB2B859B600DE68C0255BF580D5C22D02EB97AFCE474473045022100D14B60BC6E01E5C19471F87EB00A4BFCA16D039BB91AEE12DA1142E8C4CAE7C2022020E2809CF24DE2BC0C3DCF1A07C469DB415F880485B2B323E5B5AA1D9F6F22D48114AFD96601692A6C6416DBA294F0DA684675A824B28314AFD96601692A6C6416DBA294F0DA684675A824B20112300000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD401E5C92828261DBAAC933B6309C6F5C72AF020AFD4FF100000000000000000000000000000000000000000300000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD401E5C92828261DBAAC933B6309C6F5C72AF020AFD4FF01A034782E2DBAC4FB82B601CD50421E8EF24F3A00100000000000000000000000000000000000000000300000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD401E5C92828261DBAAC933B6309C6F5C72AF020AFD400'
      ]
      series = build_tx_blob_submission_series remote, txns_to_submit
      async.series series, ->
        hash = '01030E435C2EEBE2689FED7494BC159A4C9B98D0DF0B23F7DFCE223822237E8C'
        remote.request_tx hash, (e, m) ->
          meta = new Meta(m.meta)
          zero = Amount.from_json(0)
          discrepancy = compute_xrp_discrepancy(m.Fee, meta)
          assert discrepancy.equals(zero), discrepancy.to_text_full()
          done()

  suite 'RIPD 304', ->
    get_context = server_setup_teardown({server_opts: {ledger_file: 'ledger-7145315.json'}})
    test 'B1A305038D43BCDF3EA1D096E6A0ACC5FB0ECAE0C8F5D3A54AD76A2AA1E20EC4', (done) ->
      {remote} = get_context()

      txns_to_submit = [
        '120008228000000024000030FD2019000030F0201B006D076B68400000000000000F732103325EB29A014DDE22289D0EA989861D481D54D54C727578AB6C2F18BC342D3829744630440220668D06C44144C284E0346EE7785EB41B72EDBB244FE6EE02F317011A07023C63022067A52367AC01941A3FE19477D7F588C862704A44A8A771BCAD6B7A9119B71E9E8114A7C1C74DADB3693C199888A901FC2B7FD0884EE1'
        '1200002200020000240000163161D551C37937E080000000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD468400000000000000A69D4D0BEC6A319514D00000000000000000000000055534400000000000A20B3C85F482532A9578DBB3950B85CA06594D173210342D49ADE3DBC5E8E25F02B4FBB169448157B016BA203A268C3E8CCC9DF1AE39F74463044022069A2B0F79A042CC65C7CCFDF610DEAD8FDA12F53E43061F9F75FAD5B398E657A02200A4A45BB4F31E922A52F843D5CE96D83446992A13393871C31FCD8A52AE4329F81148C4BE4DBAA81F7BC66720E5874EBD2D90C9563EA83148C4BE4DBAA81F7BC66720E5874EBD2D90C9563EA0112100000000000000000000000000000000000000000300000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD401E5C92828261DBAAC933B6309C6F5C72AF020AFD4FF300000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD401E5C92828261DBAAC933B6309C6F5C72AF020AFD4FF01585E1F3BD02A15D6185F8BB9B57CC60DEDDB37C101DD39C650A96EDA48334E70CC4A85B8B2E8502CD3300000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD401E5C92828261DBAAC933B6309C6F5C72AF020AFD400'
        '1200082200000000240019B1A520190019B1A46840000000000000327321025718736160FA6632F48EA4354A35AB0340F8D7DC7083799B9C57C3E937D7185174463044022048B3669EDCA795A1897DA3C7328C8526940708DBB3FFAD88CA5DC22D0398A67502206B37796A743105DE05EE1A11BE017404B4F41FA17E6449E390C36F69D8907C078114AFFDCC86D33C153DA185156EB32608ACCF0BC713'
        '1200072200000000240019B1A664D550AF2D90A009D80000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD46540000002540BE4006840000000000000327321025718736160FA6632F48EA4354A35AB0340F8D7DC7083799B9C57C3E937D71851744630440220509F09B609573BC8ADDD55449DBD5201A40F6C1C3AA2D5D984ACB54E0F651F2E022019E6AF2937A5E76D8C9A2B5B0C4704D6BE637AAC17F2EE135DA449B0892B728B8114AFFDCC86D33C153DA185156EB32608ACCF0BC713'
        # This is the nasty one ;)
        '1200002200020000240000163261D551C37937E080000000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD468400000000000000A69D4D0BEC6A319514D00000000000000000000000055534400000000000A20B3C85F482532A9578DBB3950B85CA06594D173210342D49ADE3DBC5E8E25F02B4FBB169448157B016BA203A268C3E8CCC9DF1AE39F74473045022100C79C86BD18BBBC0343F0EB268A7770FDAEC30748ECCB9A6483E2B11488749DC00220544A5FF2D085FA5DD2A003AA9C3F031B8FE3FD4A443B659B9EE84E165795BC0581148C4BE4DBAA81F7BC66720E5874EBD2D90C9563EA83148C4BE4DBAA81F7BC66720E5874EBD2D90C9563EA0112100000000000000000000000000000000000000000300000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD401E5C92828261DBAAC933B6309C6F5C72AF020AFD4FF300000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD401E5C92828261DBAAC933B6309C6F5C72AF020AFD4FF01585E1F3BD02A15D6185F8BB9B57CC60DEDDB37C101DD39C650A96EDA48334E70CC4A85B8B2E8502CD3300000000000000000000000004A50590000000000E5C92828261DBAAC933B6309C6F5C72AF020AFD401E5C92828261DBAAC933B6309C6F5C72AF020AFD400'
      ]

      series = build_tx_blob_submission_series remote, txns_to_submit

      async.series series, ->
        hash = 'B1A305038D43BCDF3EA1D096E6A0ACC5FB0ECAE0C8F5D3A54AD76A2AA1E20EC4'
        remote.request_tx hash, (e, m) ->
          meta = new Meta(m.meta)

          expected = {
            "003313896DA56CFA0996B36AF066589EF0E689230E67DA01D13320289C834A93": {
              "pays_executed": "955.967853/XRP",
              "gets_executed": "445.6722130686/JPY/rMAz5ZnK73nyNUL4foAvaxdreczCkG3vA6",
              "pays_final": "245,418.978522/XRP",
              "gets_final": "114414.3277869564/JPY/rMAz5ZnK73nyNUL4foAvaxdreczCkG3vA6",
              "owner": "rQLEvSnbgpcUXkTU8VvSzUPX4scukCjuzb"

            },
            "9847793D6B936812907ED58455FBA4195205ABCACBE28DF9584C3A195A221E59": {
              "pays_executed": "4.19284145965/USD/rvYAfWj5gh67oV6fW32ZzP3Aw4Eubs59B",
              "gets_executed": "955.967853/XRP",
              "pays_final": "13630.84998220238/USD/rvYAfWj5gh67oV6fW32ZzP3Aw4Eubs59B",
              "gets_final": "3,107,833.795934/XRP",
              "owner": "rEhKZcz5Ndjm9BzZmmKrtvhXPnSWByssDv"
            }
          }
          ## rhsxr2aAddyCKx5iZctebT4Padxv6iWDxb
          assert.deepEqual executed_offers(meta), expected
          done()