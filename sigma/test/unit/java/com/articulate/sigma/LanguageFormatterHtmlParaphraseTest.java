package com.articulate.sigma;

import org.junit.Test;

import static org.junit.Assert.assertEquals;

/**
 * LanguageFormatter tests specifically targeted toward the htmlParaphrase( ) method.
 */
public class LanguageFormatterHtmlParaphraseTest extends UnitTestBase  {

    @Test
    public void testHtmlParaphraseDomainDatePhysical()     {
        String stmt = "(domain date 1 Physical)";

        String expectedResult = "the number 1 argument of date is an instance of physical";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Currently the output is not very good.
     * Better output: If RELATION is an instance of object attitude and the RELATION of another entity to a third entity, then the third
     * entity is an instance of physical.
     * Ideal output: Relations that are instances of object attitude have as their second argument instances that are physical things.
     */
    @Test
    public void testHtmlParaphraseInstanceRELObjectAttitude()     {
        String stmt = "(=> (and (instance ?REL ObjectAttitude) (?REL ?AGENT ?THING)) (instance ?THING Physical))";

        String expectedResult = "if an entity is an instance of object attitude and the entity another entity and a third entity, " +
                "then the third entity is an instance of physical";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseSubstanceAttributePhysicalState()     {
        String stmt = "(<=> (instance ?OBJ Substance) (exists (?ATTR) (and (instance ?ATTR PhysicalState) (attribute ?OBJ ?ATTR))))";

        String expectedResult = "an object is an instance of substance if and only if there exists an entity such that " +
                "the entity is an instance of physical state and the entity is an attribute of the object";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseBiologicallyActiveSubstance()     {
        String stmt = "(subclass BiologicallyActiveSubstance Substance)";

        String expectedResult = "biologically active substance is a subclass of substance";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphrasePureSubstanceMixture()     {
        String stmt = "(partition Substance PureSubstance Mixture)";

        String expectedResult = "substance is exhaustively partitioned into pure substance and mixture";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal output: Should have a space after the comma.
     */
    @Test
    public void testHtmlParaphrasePartlyLocated()     {
        String stmt = "(=> (and (instance ?OBJ1 Object) (partlyLocated ?OBJ1 ?OBJ2)) (exists (?SUB) (and (part ?SUB ?OBJ1) (located ?SUB ?OBJ2))))";

        String expectedResult = "if a physical is an instance of object and the physical is partly located in an object, " +
                "then there exists another physical such that the other physical is a part of the physical and the other physical is located at the object";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal output: Correct the spacing.
     */
    @Test
    public void testHtmlParaphraseDefinePhysical()     {
        String stmt = "(<=> (instance ?PHYS Physical) (exists (?LOC ?TIME) (and (located ?PHYS ?LOC) (time ?PHYS ?TIME))))";

        String expectedResult = "a physical is an instance of physical if and only if there exist an entity and " +
                "a time position such that the physical is located at the entity and the physical exists during the time position";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    // It seems this is not a correct SUO-Kif expression since only instances can be arguments in a case role.
    // Should this test therefore be removed?
    @Test
    public void testHtmlParaphrasePatient()     {
        String stmt = "( patient Leaving ?ENTITY )";

        String expectedResult = "an entity is a patient of Leaving";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseNames()     {
        String stmt = "(names \"John\" ?H)";

        String expectedResult = "an entity has name \"John\"";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseSubclassNot()     {
        String stmt = "(not (subclass ?X Animal))";

        String expectedResult = "a set or class is not a subclass of animal";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseNamesNot()     {
        String stmt = "(not (names \"John\" ?H))";

        String expectedResult = "an entity doesn't have name \"John\"";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseDrivingNot()     {
        String stmt =       "(not \n" +
                "               (exists (?D ?H)\n" +
                "                   (and\n" +
                "                       (instance ?D Driving)\n" +
                "                       (instance ?H Human)\n" +
                "                       (agent ?D ?H))))";

        //String expectedResult = "there exist a process and an agent such that the process is an instance of driving and " +
        //        "the agent is an instance of human and the agent is an agent of the process";
        String expectedResult = "there don't exist a process and an agent such that the process is an instance of driving and the agent is an instance of human and the agent is an agent of the process";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseDriving1()     {
        String stmt =       "(exists (?D ?H)\n" +
                "               (and\n" +
                "                   (instance ?D Driving)\n" +
                "                   (instance ?H Human)\n" +
                "                   (agent ?D ?H)))";

        //String expectedResult = "there exist a process and an agent such that the process is an instance of driving and " +
        //        "the agent is an instance of human and the agent is an agent of the process";
        String expectedResult = "a human drives";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseDriving2()     {
        String stmt =   "(exists (?D ?H)\n" +
                "           (and\n" +
                "           (instance ?D Driving)\n" +
                "           (instance ?H Human)\n" +
                "           (names \"John\" ?H)\n" +
                "           (agent ?D ?H)))";

        String expectedResult = "there exist a process and an agent such that the process is an instance of driving and " +
                "the agent is an instance of human and the agent has name \"John\" and the agent is an agent of the process";

        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseDrivingNot2()     {
        String stmt =   "(not\n" +
                "           (exists (?D ?H)\n" +
                "               (and\n" +
                "               (instance ?D Driving)\n" +
                "               (instance ?H Human)\n" +
                "               (names \"John\" ?H)\n" +
                "               (agent ?D ?H))))";

        String expectedResult = "there don't exist a process and an agent such that the process is an instance of driving and the agent is an instance of human and the agent has name \"John\" and the agent is an agent of the process";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseDriving3()     {
        String stmt =   "(exists (?D ?H ?Car)\n" +
                "           (and\n" +
                "           (instance ?D Driving)\n" +
                "           (instance ?H Human)\n" +
                "           (names \"John\" ?H)\n" +
                "           (instance ?Car Automobile)\n" +
                "           (agent ?D ?H)\n" +
                "           (instrument ?D ?Car)))";

        String expectedResult = "there exist a process, an agent and an object such that the process is an instance of driving and " +
                "the agent is an instance of human and the agent has name \"John\" and " +
                "the object is an instance of Automobile and the agent is an agent of the process and " +
                "the object is an instrument for the process";

        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseSubclassIf()     {
        String stmt =   "(=> " +
                "           (subclass ?Cougar Feline) " +
                "           (subclass ?Cougar Carnivore))";

        String expectedResult = "if a set or class is a subclass of feline, then the set or class is a subclass of carnivore";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseSubclassMonthFn()     {
        String stmt =   "(exists (?M) " +
                "           (time JohnsBirth (MonthFn ?M (YearFn 2000))))";

        String expectedResult = "there exists a kind of month such that JohnsBirth exists during the month a kind of month";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * See what happens when you call htmlParaphrase( ) with a syntactically incorrect statement.
     * TODO: Perhaps this test should expect an exception, but currently the exception is being swallowed--see the output
     * in the console when you run this test.
     */
    @Test
    public void testWrongNbrParens()     {
        System.out.println("\nAbout to perform unit test that throws an IndexOutOfBoundsException.");
        System.out.flush();
        String stmt =   "(exists (?Place) " +
                "           (=> " +
                                // The next line has too many right parens.
                "               (instance (GovernmentFn ?Place) StateGovernment)) " +
                "               (instance ?Place StateOrProvince))) ";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
 //       String expectedResult = "there exists a geopolitical area such that the geopolitical area is an instance of state or province";
        String expectedResult = "";
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
        System.out.println("Finished performing unit test that throws an IndexOutOfBoundsException.");
        System.out.flush();
    }

    @Test
    public void testHtmlParaphraseTypesGovFn()     {
        String stmt =   "(=> " +
                "           (instance (GovernmentFn ?Place) StateGovernment) " +
                "           (instance ?Place StateOrProvince)) ";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        String expectedResult = "if the government of a geopolitical area is an instance of StateGovernment, then the geopolitical area is an instance of state or province";
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    @Test
    public void testHtmlParaphraseElementSetIf()     {
        String stmt =   "(=> " +
                "           (forall (?ELEMENT) " +
                "               (<=> " +
                "                   (element ?ELEMENT ?SET1) " +
                "                   (element ?ELEMENT ?SET2))) " +
                "           (equal ?SET1 ?SET2))";

        String expectedResult = "if for all an entity the entity is an element of a set if and only if the entity is an element of another set, " +
                "then the set is equal to the other set";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "The document was not classified as top secret before 2001."
     */
    @Test
    public void testHtmlParaphraseNotClassifiedBefore()     {
        String stmt =   "(not \n" +
                "              (exists \n" +
                "                (?agent ?document ?event) \n" +
                "                (and \n" +
                "                  (holdsDuring \n" +
                "                  (EndFn \n" +
                "                    (WhenFn ?event)) \n" +
                "                  (attribute ?document USTopSecret)) \n" +
                "                  (lessThan \n" +
                "                  (BeginFn \n" +
                "                    (WhenFn \n" +
                "                      (attribute ?document USTopSecret))) \n" +
                "                  (BeginFn \n" +
                "                    (YearFn 2001))) \n" +
                "                  (instance ?agent Agent) \n" +
                "                  (instance ?document FactualText) \n" +
                "                  (instance ?event Classifying) \n" +
                "                  (agent ?event ?agent) \n" +
                "                  (patient ?event ?document))))";

        String expectedResult = "there don't exist an agent, an object and a physical such that USTopSecret is an attribute of the object " +
                "holds during the end of the time of existence of the physical and the beginning of the time of existence of USTopSecret " +
                "is an attribute of the object is less than the beginning of the year 2001 and the agent is an instance of agent and " +
                "the object is an instance of FactualText and the physical is an instance of classifying and the agent is an agent of the physical and " +
                "the object is a patient of the physical";
//        String expectedResult = "there don't exist an agent, an object and a physical such that an agent classifies a factualtext";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "The document was classified as top secret before 2001."
     */
    @Test
    public void testHtmlParaphraseClassifiedBefore()     {
        String stmt =   "(exists \n" +
                "              (?agent ?document ?event) \n" +
                "              (and \n" +
                "                (holdsDuring \n" +
                "                  (EndFn \n" +
                "                  (WhenFn ?event)) \n" +
                "                  (attribute ?document USTopSecret)) \n" +
                "                (lessThan \n" +
                "                  (BeginFn \n" +
                "                  (WhenFn \n" +
                "                    (attribute ?document USTopSecret))) \n" +
                "                  (BeginFn \n" +
                "                  (YearFn 2001))) \n" +
                "                (instance ?agent Agent) \n" +
                "                (instance ?document FactualText) \n" +
                "                (instance ?event Classifying) \n" +
                "                (agent ?event ?agent) \n" +
                "                (patient ?event ?document)))";

        String expectedResult = "there exist an agent, an object and a physical such that USTopSecret is an attribute of the object holds during the end of the time of existence of the physical and the beginning of the time of existence of USTopSecret is an attribute of the object is less than the beginning of the year 2001 and the agent is an instance of agent and the object is an instance of FactualText and the physical is an instance of classifying and the agent is an agent of the physical and the object is a patient of the physical";

        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "Bob sent a card."; also "A card was sent by Bob."
     */
    @Test
    public void testHtmlParaphraseBobSendCard()     {
        String stmt =   "(exists\n" +
                "              (?card ?event)\n" +
                "              (and\n" +
                "                (instance Mary-1 Human)\n" +
                "                (instance Robert-1 Human)\n" +
                "                (instance ?card BankCard)\n" +
                "                (agent ?event Robert-1)\n" +
                "                (instance ?event Directing)\n" +
                "                (patient ?event ?card)))";

        //String expectedResult = "there exist an entity and a process such that Female is an attribute of Mary-1 and Male is an attribute of Robert-1 and Mary-1 is an instance of human and Robert-1 is an instance of human and the entity is an instance of BankCard and Robert-1 is an agent of the process and the process ends at Mary-1 and the process is an instance of directing and the entity is a patient of the process";
        String expectedResult = "Robert-1 directs BankCard";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "Bob sent a card to Mary."; also "A card was sent to Mary by Bob."
     */
    @Test
    public void testHtmlParaphraseBobSendCardMary()     {
        String stmt =   "(exists\n" +
                "              (?card ?event)\n" +
                "              (and\n" +
                "                (instance Mary-1 Human)\n" +
                "                (instance Robert-1 Human)\n" +
                "                (instance ?card BankCard)\n" +
                "                (agent ?event Robert-1)\n" +
                "                (destination ?event Mary-1)\n" +
                "                (instance ?event Directing)\n" +
                "                (patient ?event ?card)))";

        String expectedResult = "there exist an entity and a process such that Mary-1 is an instance of human and Robert-1 is an instance of human and the entity is an instance of BankCard and Robert-1 is an agent of the process and the process ends at Mary-1 and the process is an instance of directing and the entity is a patient of the process";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "The man Bob sent a card to the woman Mary."; also "A card was sent to Mary by Bob."
     */
    @Test
    public void testHtmlParaphraseManBobSendCardWomanMary()     {
        String stmt =   "(exists\n" +
                "              (?card ?event)\n" +
                "              (and\n" +
                "                (attribute Mary-1 Female)\n" +
                "                (attribute Robert-1 Male)\n" +
                "                (instance Mary-1 Human)\n" +
                "                (instance Robert-1 Human)\n" +
                "                (instance ?card BankCard)\n" +
                "                (agent ?event Robert-1)\n" +
                "                (destination ?event Mary-1)\n" +
                "                (instance ?event Directing)\n" +
                "                (patient ?event ?card)))";

        String expectedResult = "there exist an entity and a process such that Female is an attribute of Mary-1 and Male is an attribute of Robert-1 and Mary-1 is an instance of human and Robert-1 is an instance of human and the entity is an instance of BankCard and Robert-1 is an agent of the process and the process ends at Mary-1 and the process is an instance of directing and the entity is a patient of the process";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "The city was built."
     */
    @Test
    public void testHtmlParaphraseCityBeBuilt()     {
        String stmt =   "(exists \n" +
                "              (?agent ?city ?event) \n" +
                "              (and \n" +
                "                (instance ?agent Agent) \n" +
                "                (instance ?city City) \n" +
                "                (instance ?event Making) \n" +
                "                (agent ?event ?agent) \n" +
                "                (patient ?event ?city)))";

        //String expectedResult = "there exist an agent, an entity and a process such that the agent is an instance of agent and the entity is an instance of city and the process is an instance of making and the agent is an agent of the process and the entity is a patient of the process";
        String expectedResult = "an agent makes a city";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "Bob eats and drinks on the desk."
     */
    @Test
    public void testHtmlParaphraseBobEatsDrinksDesk()     {
        String stmt =   "(exists \n" +
                "              (?desk ?event1 ?event2) \n" +
                "              (and \n" +
                "                (attribute Robert-1 Male) \n" +
                "                (exists \n" +
                "                  (?location) \n" +
                "                  (and \n" +
                "                  (located ?event2 ?location) \n" +
                "                  (orientation ?location ?desk On))) \n" +
                "                (instance Robert-1 Human)     \n" +
                "                (instance ?desk Desk) \n" +
                "                (agent ?event1 Robert-1) \n" +
                "                (instance ?event1 Eating) \n" +
                "                (agent ?event2 Robert-1) \n" +
                "                (instance ?event2 Drinking)))";

        String expectedResult = "there exist an object, a process and a physical such that Male is an attribute of Robert-1 and there exists another object such that the physical is located at the other object and the other object is On to the object and Robert-1 is an instance of human and the object is an instance of Desk and Robert-1 is an agent of the process and the process is an instance of eating and Robert-1 is an agent of the physical and the physical is an instance of drinking";

        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "If John sees a hamburger then he wants it."
     */
    @Test
    public void testHtmlParaphraseIfJohnSeeHamburgerThenWants()     {
        String stmt =   "(forall \n" +
                "              (?event ?hamburger) \n" +
                "              (=> \n" +
                "                (and \n" +
                "                  (attribute John-1 Male) \n" +
                "                  (instance John-1 Human) \n" +
                "                  (experiencer ?event John-1) \n" +
                "                  (instance ?event Seeing) \n" +
                "                  (instance ?hamburger Food) \n" +
                "                  (patient ?event ?hamburger)) \n" +
                "                (and \n" +
                "                  (attribute John-1 Male) \n" +
                "                  (instance John-1 Human) \n" +
                "                  (instance ?hamburger Object) \n" +
                "                  (wants John-1 ?hamburger))))";

        String expectedResult = "for all a process and a physical if Male is an attribute of John-1 and John-1 is an instance of human and John-1 experiences the process and the process is an instance of seeing and the physical is an instance of Food and the physical is a patient of the process, then Male is an attribute of John-1 and John-1 is an instance of human and the physical is an instance of object and John-1 wants the physical";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "John owns a dog."
     */
    @Test
    public void testJohnOwnsDog()     {
        String stmt =   "(exists (?dog)\n" +
                "              (and\n" +
                "               (instance ?dog Canine)\n" +
                "               (instance John-1 Human)\n" +
                "               (attribute John-1 Male)\n" +
                "               (possesses John-1 ?dog)))";

        String expectedResult = "there exists an object such that the object is an instance of canine and John-1 is an instance of human and Male is an attribute of John-1 and John-1 possesses the object";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "John gives the bank card to Mary."
     */
    @Test
    public void testJohnGivesCardMary()     {
        String stmt =   "(exists (?card ?event)\n" +
                "              (and\n" +
                "               (instance ?event Giving)\n" +
                "               (instance John-1 Human)\n" +
                "               (agent ?event John-1)\n" +
                "               (instance ?card BankCard)\n" +
                "               (patient ?event ?card)\n" +
                "               (instance Mary-1 Human)\n" +
                "               (destination ?event Mary-1)))";

        String expectedResult = "there exist an entity and a process such that the process is an instance of giving and John-1 is an instance of human and John-1 is an agent of the process and the entity is an instance of BankCard and the entity is a patient of the process and Mary-1 is an instance of human and the process ends at Mary-1";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "The man John gives the card to Mary."
     */
    @Test
    public void testManJohnGivesCardWomanMary()     {
        String stmt =   "(exists (?card ?event)\n" +
                "              (and\n" +
                "               (instance ?event Giving)\n" +
                "               (attribute John-1 Male)\n" +
                "               (instance John-1 Human)\n" +
                "               (agent ?event John-1)\n" +
                "               (instance ?card BankCard)\n" +
                "               (patient ?event ?card)\n" +
                "               (attribute Mary-1 Female)\n" +
                "               (instance Mary-1 Human)\n" +
                "               (destination ?event Mary-1)))";

        String expectedResult = "there exist an entity and a process such that the process is an instance of giving and Male is an attribute of John-1 and John-1 is an instance of human and John-1 is an agent of the process and the entity is an instance of BankCard and the entity is a patient of the process and Female is an attribute of Mary-1 and Mary-1 is an instance of human and the process ends at Mary-1";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "The oldest dog enters the bank."
     */
    @Test
    public void testOldestDogEntersBank()     {
        String stmt =   "(exists \n" +
                "                  (?bank ?dog ?event) \n" +
                "                  (and \n" +
                "                    (forall \n" +
                "                      (?X) \n" +
                "                      (=> \n" +
                "                        (and \n" +
                "                          (instance ?X Canine) \n" +
                "                          (not \n" +
                "                            (equal ?X ?dog))) \n" +
                "                        (and \n" +
                "                          (greaterThan ?val1 ?val2) \n" +
                "                          (age ?dog ?val1) \n" +
                "                          (age ?X ?val2)))) \n" +
                "                    (instance ?bank Bank-FinancialOrganization) \n" +
                "                    (instance ?dog Canine) \n" +
                "                    (instance ?event Motion) \n" +
                "                    (patient ?event ?bank) \n" +
                "                    (agent ?event ?dog)))";

        String expectedResult = "there exist an entity, an agent and a process such that for all an object if the object is an instance of canine and the object is not equal to the agent, then a quantity is greater than another quantity and the age of the agent is the quantity and the age of the object is the other quantity and the entity is an instance of Bank-FinancialOrganization and the agent is an instance of canine and the process is an instance of motion and the entity is a patient of the process and the agent is an agent of the process";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "Mr Miller enters the bank."
     */
    @Test
    public void testMrMillerEntersBank()     {
        String stmt =   "(exists \n" +
                "                (?bank ?event) \n" +
                "                (and \n" +
                "                  (attribute MrMiller FullyFormed) \n" +
                "                  (attribute MrMiller Male) \n" +
                "                  (instance MrMiller Human) \n" +
                "                  (names MrMiller \"\"MrMiller\"\") \n" +
                "                  (instance ?bank Bank-FinancialOrganization) \n" +
                "                  (agent ?event MrMiller) \n" +
                "                  (instance ?event Motion) \n" +
                "                  (patient ?event ?bank)))";

        String expectedResult = "there exist an entity and a process such that FullyFormed is an attribute of MrMiller and Male is an attribute of MrMiller and MrMiller is an instance of human and \"\"\"\" has name MrMiller and the entity is an instance of Bank-FinancialOrganization and MrMiller is an agent of the process and the process is an instance of motion and the entity is a patient of the process";

        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "He travels to (the) Sudan."
     */
    @Test
    public void testHumanTravels()     {
        String stmt =   "(exists (?he ?event)\n" +
                "                  (and\n" +
                "                    (instance ?event Transportation)\n" +
                "                    (instance ?he Human)\n" +
                "                    (agent ?event ?he)))";

        //String expectedResult = "there exist an agent and a process such that the process is an instance of transportation and Male is an attribute of the agent and the agent is an instance of human and the agent is an agent of the process and the process ends at Sudan";
        String expectedResult = "a human performs a transportation";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: "He travels to (the) Sudan."
     */
    @Test
    public void testHeTravelsSudan()     {
        String stmt =   "(exists (?he ?event)\n" +
                "                  (and\n" +
                "                    (instance ?event Transportation)\n" +
                "                    (attribute ?he Male)\n" +
                "                    (instance ?he Human)\n" +
                "                    (agent ?event ?he)\n" +
                "                    (destination ?event Sudan)))";

        String expectedResult = "there exist an agent and a process such that the process is an instance of transportation and Male is an attribute of the agent and the agent is an instance of human and the agent is an agent of the process and the process ends at Sudan";
        //String expectedResult = "there exist an agent and a process such that a human performs a transportation";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * Ideal: ? "If an animal performs an intentional process, then the animal is awake."
     * This test may fail, but should be fixed by #17181: Modify LanguageFormatter.computeVariableTypes( )
     * so that the HashSet consists of only a single element which is the least general--the most specific.
     */
    @Test
    public void testAwake()     {
        String stmt =   "(=>\n" +
                "           (and\n" +
                "               (instance ?PROC IntentionalProcess)\n" +
                "               (agent ?PROC ?HUMAN)\n" +
                "               (instance ?HUMAN Animal))\n" +
                "           (holdsDuring\n" +
                "               (WhenFn ?PROC)\n" +
                "               (attribute ?HUMAN Awake)))";

        String expectedResult = "if a physical is an instance of intentional process and an agent is an agent of the physical and the agent is an instance of animal, " +
                "then awake is an attribute of the agent holds during the time of existence of the physical";
//        String expectedResult = "if a process is an instance of intentional process and an agent is an agent of the process and the agent is an instance of animal, " +
//                "then awake is an attribute of the agent holds during the time of existence of the process";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

    /**
     * NOTE: Currently this test verifies that LanguageFormatter recovers from an IllegalArgumentException thrown by SumoProcessCollector. The console will display
     * a message to that effect.
     * FIXME: We need to find a better way to verify that LanguageFormatter is recovering from the exception. This must be done whenever we become
     * able to correctly translate this input into natural language.
     */
    @Test
    public void testAnimalLanguage()     {
        String stmt =   "(=>\n" +
                "           (and\n" +
                "               (instance ?LANG AnimalLanguage)\n" +
                "               (agent ?PROC ?AGENT)\n" +
                "               (instrument ?PROC ?LANG))\n" +
                "           (and\n" +
                "               (instance ?AGENT Animal)\n" +
                "               (not\n" +
                "                   (instance ?AGENT Human))))";

        String expectedResult = "if an object is an instance of animal language and an agent is an agent of a process and the object is an instrument for the process, " +
                "then the agent is an instance of animal and the agent is not an instance of human";
        String actualResult = LanguageFormatter.htmlParaphrase("", stmt, SigmaTestBase.kb.getFormatMap("EnglishLanguage"),
                SigmaTestBase.kb.getTermFormatMap("EnglishLanguage"),
                SigmaTestBase.kb, "EnglishLanguage");
        assertEquals(expectedResult, LanguageFormatter.filterHtml(actualResult));
    }

}