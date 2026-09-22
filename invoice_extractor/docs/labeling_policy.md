# Labeling policy

Every ambiguous call in the test set is decided here. When a new page raises a
question this file does not answer, add the rule here *before* labeling it --
otherwise the labels drift and the metric stops meaning anything.

## 1. Label what is printed

Record only values visible on the page. Never compute a missing one.

If an invoice shows line items but no subtotal, `totals.subtotal` is `null`,
even though the items obviously sum to something. A null the validator can
derive is more useful than a guess baked into ground truth: it lets us measure
reading and arithmetic separately.

Corollary: when the page's own arithmetic is wrong, label the wrong value.
`invoice-001` states a tax of 26.56 that does not match its own items. That is
what the page says, so that is the label. The validator's job is to flag it.

## 2. items[] is things bought or charged

Discount rows and tax rows go to `totals`, even when they are printed inside
the table body. `invoice-001` prints "New client discount" and "Tax (4.25%...)"
as table rows; they are still `totals.discount` and `totals.tax_lines`.

Rationale: "how many items are on this invoice" should answer the same way
regardless of where the template chose to draw the discount. Without this rule
`item_count` is not comparable across templates.

Restated subtotals inside the table ("Total Account Charges" on `invoice-006`)
are not items either.

## 3. Money

Floats in the invoice currency, 2 decimal places. Parenthesised amounts are
negative in the source, but `totals.discount` stores a **positive magnitude**:
`(50.00)` on the page becomes `"discount": 50.00`.

## 4. description is the NAME

`description` is the item's name. Secondary prose in its own column goes to
`details`; an item/part/HS code goes to `sku`.

On `invoice-006` the columns are `Item | Qty | HS Code | Description | Total`,
so `description = "Mahogany Table"`, `details = "Dark wood mahogany"`,
`sku = "MG6392"` -- note the column literally headed "Description" is *not* the
`description` field.

## 5. unit is split out of quantity

A cell reading `10 pcs` becomes `quantity: 10, unit: "pcs"`. A cell reading
`1 hour` becomes `quantity: 1, unit: "hour"`. Units are compared through a
synonym table (`evaluation/normalize.py`), so `ea`/`each`/`pcs` all agree.

## 6. total vs amount_due

`total` is the final payable amount **as presented, however it is labelled**.
`amount_due` is filled only when the page prints a distinct amount-due or
balance-due line.

`invoice-006` has no row called "Total" -- its final figure is
`Amount Due (USD) $5400`. That fills both fields. Do not leave `total` null on
a technicality; a downstream consumer asking "what do I owe" must always find
an answer in `total`.

## 7. Tax can be several lines

`totals.tax_lines[]` holds each printed tax line; `tax_total` is their sum.
`invoice-003` charges GST at 10% on one item and 20% on another, giving
`tax_total = 410.00`. A single scalar tax field cannot represent that page,
which is why the schema has a list.

## 8. Seller vs buyer

`seller` is the organisation **issuing** the invoice. `buyer` is whoever
appears under Bill To / Sent To / Ship To / TO.

This is the most common extraction bug, and the set is built to catch it:
`invoice-001` prints `[Company Name]` in *both* roles, and `invoice-005` uses
`SENT TO` rather than `BILL TO`.

When a seller field is printed but blank (`invoice-002`), the label is `null`.
Extracting anything there is a hallucination and is scored as one.

## 9. Logos

`belongs_to_issuer` distinguishes a real issuer logo from a template vendor's
mark. The only logo on `invoice-002` is "OnlineInvoices" -- the company that
made the template, not the company sending the invoice. Decorative imagery
(the Australian flag on `invoice-002`) is not a logo.

`box` is left `null` for now, deliberately. Eyeballed boxes would silently
corrupt the IoU metric later; they get annotated properly in Phase 4 with a
real annotation tool.

## 10. Dates

ISO 8601 (`YYYY-MM-DD`). Where day-first vs month-first is ambiguous, use
another date on the same page to disambiguate and record the reasoning in
`notes`. Where a date is visually truncated and unrecoverable
(`invoice-002` shows `12/26/202`), the label is `null`.

## 11. notes is never scored

Free text for the labeler. Use it to record why an ambiguous call was made --
future-you will need it.

---

## Provenance warning

The six labels in `data/labels/` were **drafted by Claude reading the images**
and have not yet been verified by a human.

Until they are, the metric measures *agreement with Claude*, not correctness --
and since the Phase 2 extractor is also Claude, an unverified label set would
flatter it. Verify every field against the image before trusting a score, and
delete this section when you have.
