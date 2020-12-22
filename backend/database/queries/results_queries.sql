-- :name get_ref_results :many
SELECT *
FROM ref_results
WHERE submission_id = :submission_id;


-- :name insert_ref_result :scalar
INSERT INTO ref_results(submission_id, reference_id)
VALUES (:submission_id, :reference_id)
RETURNING id;

-- :name update_ref_result
UPDATE ref_results
SET result     = :result,
    sub_stdout = :sub_stdout,
    sub_stderr = :sub_stderr,
    ref_stdout = :ref_stdout,
    ref_stderr = :ref_stderr
WHERE id = :result_id;

-- :name remove_ref_results
DELETE
FROM ref_results
WHERE submission_id = :submission_id;

-- :name remove_all_ref_results
DELETE
FROM ref_results;

-- :name remove_all_tournament_results
DELETE
FROM tournament_results;